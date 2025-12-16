#include "GAS/GA/RSGameplayAbility_Attack.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RemnantSoul/RSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Attack::URSGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("URSGameplayAbility_Attack::ActivateAbility()")));

	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

	ARSCharacter* AvatarCharacter = CastChecked<ARSCharacter>(ActorInfo->AvatarActor.Get());
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), AvatarCharacter->AttackMontage);
	PlayAttackTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
	PlayAttackTask->ReadyForActivation();
}

void URSGameplayAbility_Attack::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Attack::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("URSGameplayAbility_Attack::EndAbility()")));

	ARSCharacter* AvatarCharacter = CastChecked<ARSCharacter>(ActorInfo->AvatarActor.Get());
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}