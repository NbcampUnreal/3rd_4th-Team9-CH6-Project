#include "GAS/GA/RSGameplayAbility_Roll.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/RSCharacter.h"
#include "RSGameplayTags.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Math/RotationMatrix.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Roll)

URSGameplayAbility_Roll::URSGameplayAbility_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	AbilityTags.AddTag(Tags.Ability_Roll);
	ActivationOwnedTags.AddTag(Tags.Status_Roll);
}

void URSGameplayAbility_Roll::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ARSCharacter* Char = Cast<ARSCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 1) 입력 기반 방향 고정
	const float Degrees = ComputeRollDegreesFromInput(Char);
	Char->SetRollDirectionDegrees(Degrees);

	// 2) Roll 종료 이벤트 대기 (AnimSequence Notify -> GameplayEvent)
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
		// 이벤트 대기가 실패하면 롤 상태가 남을 수 있으니 즉시 종료로 수렴
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float URSGameplayAbility_Roll::ComputeRollDegreesFromInput(const ACharacter* Char) const
{
	if (!Char)
	{
		return 0.f;
	}

	const FVector Input = Char->GetLastMovementInputVector();
	if (Input.IsNearlyZero())
	{
		return 0.f; // 입력 없으면 전방 롤
	}

	const FRotator YawRot(0.f, Char->GetActorRotation().Yaw, 0.f);
	const FVector Forward = YawRot.Vector();
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	const FVector Input2D = Input.GetSafeNormal2D();
	const float X = FVector::DotProduct(Input2D, Forward);
	const float Y = FVector::DotProduct(Input2D, Right);

	return FMath::RadiansToDegrees(FMath::Atan2(Y, X)); // -180~180
}

