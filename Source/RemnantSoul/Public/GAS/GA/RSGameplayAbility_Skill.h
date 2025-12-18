#pragma once

#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Skill.generated.h"

class UAnimMontage;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Skill : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Skill();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveSkillActionMontage;

};
