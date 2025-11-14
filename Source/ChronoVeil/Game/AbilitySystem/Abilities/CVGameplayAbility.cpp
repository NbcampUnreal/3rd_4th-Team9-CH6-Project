#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "Game/Character/CVCharacter.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
//#include "Game/System/CVLogChannel.h"

UCVGameplayAbility::UCVGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UCVGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCVGameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled
)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ACVCharacter* UCVGameplayAbility::GetCVCharacter(const FGameplayAbilityActorInfo* ActorInfo) const
{
    return ActorInfo ? Cast<ACVCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
}

UCVAbilitySystemComponent* UCVGameplayAbility::GetCVASC(const FGameplayAbilityActorInfo* ActorInfo) const
{
    return ActorInfo ? Cast<UCVAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()) : nullptr;
}

bool UCVGameplayAbility::HasAuthority(const FGameplayAbilityActivationInfo& ActivationInfo) const
{
    return (GetCurrentActorInfo()->IsNetAuthority());
}
