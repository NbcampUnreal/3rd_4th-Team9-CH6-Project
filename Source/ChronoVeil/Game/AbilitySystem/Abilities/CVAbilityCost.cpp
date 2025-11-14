#include "Game/AbilitySystem/Abilities/CVAbilityCost.h" // 경로가 실제 파일 위치와 일치하는지 확인

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
    // 기본은 NOP
}
