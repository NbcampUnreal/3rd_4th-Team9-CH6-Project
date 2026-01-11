#include "AbilitySystem/Abilities/RSGameplayAbility.h"

#include "Character/RSCharacter.h"
#include "AbilitySystemComponent.h"

URSGameplayAbility::URSGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void URSGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void URSGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ARSCharacter* URSGameplayAbility::GetRSCharacter(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
}

UAbilitySystemComponent* URSGameplayAbility::GetRSASC(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
}

bool URSGameplayAbility::HasAuthority(const FGameplayAbilityActivationInfo& ActivationInfo) const
{
	return GetCurrentActorInfo() && GetCurrentActorInfo()->IsNetAuthority();
}
