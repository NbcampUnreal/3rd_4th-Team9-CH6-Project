#include "Game/AbilitySystem/Abilities/CVAbilityCost.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayAbilitySpec.h"

bool UCVAbilityCost::CheckCost_Implementation(const UCVGameplayAbility*,
    FGameplayAbilitySpecHandle,
    const FGameplayAbilityActorInfo&,
    FGameplayTagContainer&) const
{
    return true;
}

void UCVAbilityCost::ApplyCost_Implementation(const UCVGameplayAbility*,
    FGameplayAbilitySpecHandle,
    const FGameplayAbilityActorInfo&,
    const FGameplayAbilityActivationInfo&) const
{
    
}
