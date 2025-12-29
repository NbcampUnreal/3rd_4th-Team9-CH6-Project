// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "RS_Run_Ability.generated.h"

class UGameplayEffect;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class REMNANTSOUL_API URS_Run_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URS_Run_Ability();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Run")
	TSubclassOf<UGameplayEffect> RunSpeedGE;

	UPROPERTY(EditDefaultsOnly, Category="Run")
	TSubclassOf<UGameplayEffect> RunSpeedDecayGE;

	FActiveGameplayEffectHandle RunGEHandle;
	FActiveGameplayEffectHandle DecayGEHandle;

	UPROPERTY()
	UAbilityTask_WaitGameplayEffectRemoved* WaitDecayRemovedTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEndEventTask;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION()
	void OnEndRequested(FGameplayEventData Payload);

	UFUNCTION()
	void OnDecayFinished(const FGameplayEffectRemovalInfo& RemovalInfo);

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	
};
