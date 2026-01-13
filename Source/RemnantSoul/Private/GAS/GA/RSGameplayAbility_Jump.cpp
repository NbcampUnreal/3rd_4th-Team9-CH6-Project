// RSGameplayAbility_Jump.cpp


#include "GAS/GA/RSGameplayAbility_Jump.h"
#include "GameFramework/Character.h"
#include "GAS/AT/RSAbilityTask_JumpAndWaitForLanding.h"

#include "GameplayEffect.h"

URSGameplayAbility_Jump::URSGameplayAbility_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool URSGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bResult == false)
	{
		return false;
	}

	const ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	return (IsValid(AvatarCharacter) == true && AvatarCharacter->CanJump() == true);
}

void URSGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// -----------------------------
	// [병합 추가 1] CommitAbility
	// -----------------------------
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// -----------------------------
	// [병합 추가 2] 스태미나 비용 GE 1회 적용
	// -----------------------------
	if (JumpStaminaCostEffectClass)
	{
		const UGameplayEffect* GE = JumpStaminaCostEffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, GE, 1);
	}

	// -----------------------------
	// [기존 코드 그대로] 점프 태스크 실행
	// -----------------------------
	URSAbilityTask_JumpAndWaitForLanding* JumpAndWaitingForLandingTask = URSAbilityTask_JumpAndWaitForLanding::CreateTask(this);
	if (!JumpAndWaitingForLandingTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	JumpAndWaitingForLandingTask->OnComplete.AddDynamic(this, &ThisClass::HandleLanded);
	JumpAndWaitingForLandingTask->ReadyForActivation();
}

void URSGameplayAbility_Jump::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	ACharacter* AvatarCharacter = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (IsValid(AvatarCharacter))
	{
		AvatarCharacter->StopJumping();
	}
}

void URSGameplayAbility_Jump::HandleLanded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
