#include "GAS/GA/RSGameplayAbility_Skill.h"

#include "Character/RSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Skill::URSGameplayAbility_Skill()
{
}

void URSGameplayAbility_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ARSCharacter* AvatarCharacter = Cast<ARSCharacter>(ActorInfo->AvatarActor.Get());
	if (!AvatarCharacter)
	{
		return;
	}

	ActiveSkillActionMontage = AvatarCharacter->GetSkillActionMontage();
	if (!ActiveSkillActionMontage)
	{
		return;
	}

	Super::CommitAbility(Handle, ActorInfo, ActivationInfo);

	AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SkillMontage"), ActiveSkillActionMontage, 1.0f);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleteCallback);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterruptedCallback);

	PlayMontageTask->ReadyForActivation();
}

void URSGameplayAbility_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ARSCharacter* AvatarCharacter = Cast<ARSCharacter>(ActorInfo->AvatarActor.Get());
	if (!AvatarCharacter)
	{
		return;
	}

	AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void URSGameplayAbility_Skill::OnCompleteCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Skill::OnInterruptedCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
