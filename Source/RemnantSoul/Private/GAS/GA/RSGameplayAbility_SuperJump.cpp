#include "GAS/GA/RSGameplayAbility_SuperJump.h"
#include "GameFramework/Character.h"
#include "GAS/AT/RSAbilityTask_JumpAndWaitForLanding.h"

URSGameplayAbility_SuperJump::URSGameplayAbility_SuperJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool URSGameplayAbility_SuperJump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags
) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	return (IsValid(Character) && Character->CanJump());
}

void URSGameplayAbility_SuperJump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(Character))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 일반 점프보다 강한 점프력 적용
	const FVector LaunchVelocity = FVector(0.f, 0.f, SuperJumpStrength);
	Character->LaunchCharacter(LaunchVelocity, false, true);

	// 착지 감지 Task
	URSAbilityTask_JumpAndWaitForLanding* WaitForLandingTask = URSAbilityTask_JumpAndWaitForLanding::CreateTask(this);
	WaitForLandingTask->OnComplete.AddDynamic(this, &ThisClass::OnLanded);
	WaitForLandingTask->ReadyForActivation();
}

void URSGameplayAbility_SuperJump::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}

void URSGameplayAbility_SuperJump::OnLanded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
