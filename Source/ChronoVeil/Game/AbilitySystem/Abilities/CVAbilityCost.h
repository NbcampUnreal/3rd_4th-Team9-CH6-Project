#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayAbilitySpec.h"
#include "CVAbilityCost.generated.h"

class UCVGameplayAbility;
struct FGameplayAbilitySpecHandle;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;

UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class CHRONOVEIL_API UCVAbilityCost : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "CV|Cost")
	bool CheckCost(const UCVGameplayAbility* Ability,
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo& ActorInfo,
		FGameplayTagContainer& OptionalRelevantTags) const;

	UFUNCTION(BlueprintNativeEvent, Category = "CV|Cost")
	void ApplyCost(const UCVGameplayAbility* Ability,
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo& ActorInfo,
		const FGameplayAbilityActivationInfo& ActivationInfo) const;
};
