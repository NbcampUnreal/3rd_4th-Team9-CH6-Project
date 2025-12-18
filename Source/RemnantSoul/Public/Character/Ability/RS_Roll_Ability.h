// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RS_Roll_Ability.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class REMNANTSOUL_API URS_Roll_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URS_Roll_Ability();

protected:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	) override;

	//추후 개발을 진행하며 개선방안을 찾아봐야함.
	UPROPERTY(EditDefaultsOnly, Category="Roll|Animation")
	UAnimMontage* RollMontage;

	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> RollCostGE;

	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> InvincibleGE;

	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> RollCooldownGE;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();
	
};
