// RSGameplayAbility_Jump.cpp


#include "GAS/GA/RSGameplayAbility_Jump.h"
#include "GameFramework/Character.h"
#include "GAS/AT/RSAbilityTask_JumpAndWaitForLanding.h"

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

	URSAbilityTask_JumpAndWaitForLanding* JumpAndWaitingForLandingTask = URSAbilityTask_JumpAndWaitForLanding::CreateTask(this);
	JumpAndWaitingForLandingTask->OnComplete.AddDynamic(this, &ThisClass::OnLanded);
	JumpAndWaitingForLandingTask->ReadyForActivation();
}

void URSGameplayAbility_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ACharacter* AvatarCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	AvatarCharacter->StopJumping();
}

void URSGameplayAbility_Jump::OnLanded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
