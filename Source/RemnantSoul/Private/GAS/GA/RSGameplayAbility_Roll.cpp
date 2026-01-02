#include "GAS/GA/RSGameplayAbility_Roll.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Character/RSCharacter.h"
#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Roll)

URSGameplayAbility_Roll::URSGameplayAbility_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// "능력 자체 태그" (필요하면)
	AbilityTags.AddTag(Tags.Ability_Roll);

	// 활성 중 상태 태그(ABP 전이/무적 등 SSOT로 쓰는 용도)
	ActivationOwnedTags.AddTag(Tags.Status_Roll);

	// 무한 롤/중복 발동 방지(선택)
	// - Status_Roll이 붙어있으면 같은 롤을 또 켤 이유가 없으니 block
	BlockAbilitiesWithTag.AddTag(Tags.Ability_Roll);

	// 네트워크 정책은 프로젝트 상황에 맞춰 조정 가능
	// ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void URSGameplayAbility_Roll::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 재진입 방지(방어적으로)
	if (bRollingActive)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	bRollingActive = true;

	ARSCharacter* Char = Cast<ARSCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 낙하 중 롤 금지 같은 조건은 여기서 체크(원하면 확장)
	if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
	{
		if (Move->IsFalling())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// (추천) 롤 중 이동 입력 누적을 막아 "툭" 전진을 원천 차단
	SetMoveInputIgnored(true);

	// 1) 입력 기반 방향 고정(정지 상태에서도 방향 결정 가능)
	const float Degrees = ComputeRollDegreesFromInput(Char);
	Char->SetRollDirectionDegrees(Degrees);

	// 2) RollEnd 이벤트 대기 (RSAnimNotify_RollEnd가 쏴줄 것)
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	UAbilityTask_WaitGameplayEvent* WaitEnd =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags.Event_Roll_End, nullptr, false, false);

	if (WaitEnd)
	{
		WaitEnd->EventReceived.AddDynamic(this, &ThisClass::OnRollEndEventReceived);
		WaitEnd->ReadyForActivation();
	}
	else
	{
		// 이벤트 태스크 생성 실패면 안전 종료
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void URSGameplayAbility_Roll::OnRollEndEventReceived(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Roll::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	// 1) 롤 종료 순간 튐 방지: 입력/속도 flush
	FlushMovementAfterRoll(Char);

	// 2) 여기서 IgnoreMoveInput을 "즉시 해제"하지 않는다.
	//    (즉시 해제하면 다음 프레임에 W 입력이 들어와서 '툭' 가능)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_RestoreMoveInput);

		// 다음 틱 또는 짧은 딜레이 후에 복구
		World->GetTimerManager().SetTimer(
			TimerHandle_RestoreMoveInput,
			[this]()
			{
				if (APlayerController* PC = CachedPC.Get())
				{
					PC->SetIgnoreMoveInput(false);
				}
			},
			PostRollIgnoreMoveSeconds,
			false);
	}
	else
	{
		// 월드가 없으면 안전 복구
		if (APlayerController* PC = CachedPC.Get())
		{
			PC->SetIgnoreMoveInput(false);
		}
	}

	bRollingActive = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float URSGameplayAbility_Roll::ComputeRollDegreesFromInput(const ACharacter* Char) const
{
	if (!Char) return 0.f;

	// 정지 상태에서도 입력이 있으면 방향을 얻을 수 있음
	const FVector Input = Char->GetLastMovementInputVector();

	if (Input.IsNearlyZero())
	{
		return 0.f; // 입력 없으면 전방 롤
	}

	// 캐릭터 yaw 기준 로컬 방향 계산
	const FRotator YawRot(0.f, Char->GetActorRotation().Yaw, 0.f);
	const FVector Forward = YawRot.Vector();
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	const FVector N = Input.GetSafeNormal2D();

	const float X = FVector::DotProduct(N, Forward);
	const float Y = FVector::DotProduct(N, Right);

	// -180~180
	return FMath::RadiansToDegrees(FMath::Atan2(Y, X));
}

void URSGameplayAbility_Roll::SetMoveInputIgnored(bool bIgnored)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;

	APlayerController* PC = Cast<APlayerController>(Char->GetController());
	if (!PC) return;

	CachedPC = PC;
	PC->SetIgnoreMoveInput(bIgnored);
}

void URSGameplayAbility_Roll::FlushMovementAfterRoll(ACharacter* Char)
{
	if (!Char) return;

	Char->ConsumeMovementInputVector();

	if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->ClearAccumulatedForces(); // 추가: 남아있는 힘/가속 제거
	}
}
