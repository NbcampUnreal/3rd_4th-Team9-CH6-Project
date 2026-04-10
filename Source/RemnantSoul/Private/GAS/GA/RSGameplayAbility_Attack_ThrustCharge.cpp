#include "GAS/GA/RSGameplayAbility_Attack_ThrustCharge.h"

#include "RemnantSoul.h"
#include "RSGameplayTags.h"
#include "Character/RSCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Attack_ThrustCharge::URSGameplayAbility_Attack_ThrustCharge()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Attack_ThrustCharge::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	bIsCharging = true;
	bWantsRelease = false;
}

void URSGameplayAbility_Attack_ThrustCharge::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// 차지 중이었다면 릴리즈를 요청
	if (bIsCharging)
	{
		bWantsRelease = true;
	}
}

void URSGameplayAbility_Attack_ThrustCharge::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (!IsValid(AvatarCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* ChargeMontage = AvatarCharacter->GetAttackThrustChargeMontage();
	UAnimMontage* ReleaseMontage = AvatarCharacter->GetAttackThrustReleaseMontage();

	if (!IsValid(ChargeMontage) || !IsValid(ReleaseMontage))
	{
		UE_LOG(LogRemnantSoul, Error, TEXT("[ThrustCharge] Missing montages. Charge=%d Release=%d"),
			IsValid(ChargeMontage) ? 1 : 0, IsValid(ReleaseMontage) ? 1 : 0);

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedChargeMontage = ChargeMontage;
	CachedReleaseMontage = ReleaseMontage;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 스태미나 코스트(원하면 릴리즈 시점으로 옮겨도 됨)
	if (StaminaCostEffectClass)
	{
		const UGameplayEffect* GE = StaminaCostEffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, GE, 1);
	}

	if (UCharacterMovementComponent* MoveComp = AvatarCharacter->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_None);
	}

	// 차지 시작 시간 기록
	ChargeStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	// 차지 몽타주 재생
	TryPlayChargeMontage();

	// 릴리즈 체크 타이머 시작
	StartChargeTimer();
}

void URSGameplayAbility_Attack_ThrustCharge::TryPlayChargeMontage()
{
	if (!IsValid(CachedChargeMontage))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("PlayThrustCharge"),
			CachedChargeMontage,
			1.0f
		);

	if (!Task)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Task->OnCompleted.AddDynamic(this, &ThisClass::OnChargeCompleted);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnChargeCanceled);
	Task->ReadyForActivation();
}

void URSGameplayAbility_Attack_ThrustCharge::StartChargeTimer()
{
	if (!GetWorld())
	{
		return;
	}

	if (ChargeTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);
		ChargeTimerHandle.Invalidate();
	}

	GetWorld()->GetTimerManager().SetTimer(
		ChargeTimerHandle,
		[this]()
		{
			// 릴리즈 입력이 왔거나, 최대 차지 도달이면 릴리즈로 전환
			const float ChargeSeconds = GetCurrentChargeSeconds();
			const bool bMaxed = (ChargeSeconds >= MaxChargeSeconds);

			if (bWantsRelease || bMaxed)
			{
				// 너무 짧게 눌렀으면 무시하고 종료
				if (ChargeSeconds < MinChargeSeconds)
				{
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
					return;
				}

				// 차지 종료 → 릴리즈 1회
				bIsCharging = false;

				// 차지 몽타주 정리(현재 재생 중이면 중단)
				if (CurrentActorInfo)
				{
					if (UAnimInstance* AnimInst = CurrentActorInfo->GetAnimInstance())
					{
						AnimInst->Montage_Stop(0.10f, CachedChargeMontage);
					}
				}

				TryPlayReleaseMontage();

				// 타이머는 이제 필요 없음
				if (GetWorld())
				{
					GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);
					ChargeTimerHandle.Invalidate();
				}
			}
		},
		ChargePollInterval,
		true
	);
}

float URSGameplayAbility_Attack_ThrustCharge::GetCurrentChargeSeconds() const
{
	if (!GetWorld())
	{
		return 0.f;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	return FMath::Max(0.f, Now - ChargeStartTime);
}

void URSGameplayAbility_Attack_ThrustCharge::TryPlayReleaseMontage()
{
	if (!IsValid(CachedReleaseMontage))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 릴리즈 시작 시점에 피격 판정 이벤트를 날리는 방식(간단/즉시)
	// 더 정교하게 하려면 몽타주 AnimNotify에서 CheckHit 이벤트를 보내도록 바꾸면 됨.
	SendCheckHitEvent();

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("PlayThrustRelease"),
			CachedReleaseMontage,
			ReleasePlayRate
		);

	if (!Task)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Task->OnCompleted.AddDynamic(this, &ThisClass::OnReleaseCompleted);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnReleaseCanceled);
	Task->ReadyForActivation();
}

void URSGameplayAbility_Attack_ThrustCharge::SendCheckHitEvent() const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar))
	{
		return;
	}

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	FGameplayEventData Data;
	Data.EventTag = Tags.Event_Attack_CheckHit;

	// ThrustCharge는 콤보가 아니라 1회 강공격이므로 1로 고정
	Data.EventMagnitude = 1.0f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Avatar, Data.EventTag, Data);
}

void URSGameplayAbility_Attack_ThrustCharge::OnChargeCompleted()
{
	// 차지 몽타주가 끝까지 가버린 경우(루프가 아니거나 섹션이 짧은 경우)
	// 아직 릴리즈 입력이 안 왔으면 "대기" 상태로 두되, 타이머가 MaxCharge로 릴리즈 처리함
}

void URSGameplayAbility_Attack_ThrustCharge::OnChargeCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URSGameplayAbility_Attack_ThrustCharge::OnReleaseCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Attack_ThrustCharge::OnReleaseCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URSGameplayAbility_Attack_ThrustCharge::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UAnimInstance* AnimInst = ActorInfo ? ActorInfo->GetAnimInstance() : nullptr;
	ARSCharacter* Char = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;

	if (AnimInst && Char)
	{
		if (UAnimMontage* Charge = Char->GetAttackThrustChargeMontage())
		{
			AnimInst->Montage_Stop(0.1f, Charge);
		}
		if (UAnimMontage* Release = Char->GetAttackThrustReleaseMontage())
		{
			AnimInst->Montage_Stop(0.1f, Release);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
