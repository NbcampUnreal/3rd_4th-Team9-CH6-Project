#include "GAS/GA/RSGameplayAbility_Sprint_Active.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "RSGameplayTags.h"
#include "GAS/AT/RSAbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Sprint_Active)

URSGameplayAbility_Sprint_Active::URSGameplayAbility_Sprint_Active()
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	AbilityTags.AddTag(Tags.Ability_Sprint_Active);
	ActivationOwnedTags.AddTag(Tags.Status_Sprint);
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = false;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = Tags.Ability_Sprint_Active;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void URSGameplayAbility_Sprint_Active::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 코스트/쿨다운 체크(있다면)
	if (!K2_CheckAbilityCost() || !K2_CheckAbilityCooldown())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	ACharacter* Char = GetCharacterFromActorInfo();
	if (!Char)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UCharacterMovementComponent* Move = Char->GetCharacterMovement();
	if (!Move)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Sprint 속도 적용(서버/예측 모두 체감)
	CachedOriginalMaxWalkSpeed = Move->MaxWalkSpeed;
	Move->MaxWalkSpeed = SprintMaxWalkSpeed;
	bSpeedOverridden = true;

	// Tick(방향 변화 감시)
	if (URSAbilityTask_WaitForTick* TickTask = URSAbilityTask_WaitForTick::WaitForTick(this))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}

	// 입력 릴리즈로 종료(주의: 이벤트 트리거 활성화에서는 프로젝트 입력 바인딩 방식에 따라 안 올 수도 있음)
	if (UAbilityTask_WaitInputRelease* ReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		ReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		ReleaseTask->ReadyForActivation();
	}

	// 비용 커밋 주기
	GetWorld()->GetTimerManager().SetTimer(
		SprintTimerHandle,
		this,
		&ThisClass::OnSprintCommitTick,
		CommitInterval,
		true
	);
}

void URSGameplayAbility_Sprint_Active::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);

	// 원복
	if (bSpeedOverridden)
	{
		if (ACharacter* Char = GetCharacterFromActorInfo())
		{
			if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
			{
				Move->MaxWalkSpeed = CachedOriginalMaxWalkSpeed;
			}
		}
		bSpeedOverridden = false;
	}

	// 종료 시 회복 블락 이펙트(선택)
	if (RecoveryBlockEffectClass)
	{
		const UGameplayEffect* GE = RecoveryBlockEffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GE, 1);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URSGameplayAbility_Sprint_Active::OnTick(float DeltaTime)
{
	// 서버는 더 엄격하게 Velocity 기반 판정
	const bool bKeep =
		HasAuthority(&CurrentActivationInfo) ? IsStillForwardMoving_ServerAuth() : IsStillForwardMoving_ClientApprox();

	if (!bKeep)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// (선택) 자원 유지 체크: Attribute가 있으면 유지 조건 검사
	if (SprintCostAttribute.IsValid())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			bool bFound = false;
			const float RemainValue =
				UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(ASC, SprintCostAttribute, bFound);

			if (!bFound || RemainValue <= MinRemainValueToKeepSprint)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				return;
			}
		}
	}
}

void URSGameplayAbility_Sprint_Active::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Sprint_Active::OnSprintCommitTick()
{
	// 비용 커밋 실패하면 종료 + 쿨다운 커밋(원하면)
	if (!K2_CommitAbilityCost())
	{
		GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
		K2_CommitAbilityCooldown(false, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

bool URSGameplayAbility_Sprint_Active::IsStillForwardMoving_ServerAuth() const
{
	ACharacter* Char = GetCharacterFromActorInfo();
	if (!Char)
	{
		return false;
	}

	UCharacterMovementComponent* Move = Char->GetCharacterMovement();
	if (!Move || Move->IsFalling())
	{
		return false;
	}

	const FVector DirWS = Move->Velocity.GetSafeNormal();
	if (DirWS.IsNearlyZero())
	{
		return false;
	}

	const FVector LocalDir = Char->GetActorTransform().InverseTransformVectorNoScale(DirWS).GetSafeNormal();
	return (LocalDir.X > ForwardThreshold);
}

bool URSGameplayAbility_Sprint_Active::IsStillForwardMoving_ClientApprox() const
{
	ACharacter* Char = GetCharacterFromActorInfo();
	if (!Char)
	{
		return false;
	}

	UCharacterMovementComponent* Move = Char->GetCharacterMovement();
	if (!Move || Move->IsFalling())
	{
		return false;
	}

	// 클라는 입력(가속) 기반으로 좀 더 자연스럽게 판정
	const FVector Accel = Move->GetCurrentAcceleration();
	const FVector DirWS = !Accel.IsNearlyZero() ? Accel.GetSafeNormal() : Move->Velocity.GetSafeNormal();
	if (DirWS.IsNearlyZero())
	{
		return false;
	}

	const FVector LocalDir = Char->GetActorTransform().InverseTransformVectorNoScale(DirWS).GetSafeNormal();
	return (LocalDir.X > ForwardThreshold);
}

ACharacter* URSGameplayAbility_Sprint_Active::GetCharacterFromActorInfo() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}
