// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RS_ShowVictoryWidgetAbility.generated.h"

class URS_VictoryWidget;

UCLASS()
class REMNANTSOUL_API URS_ShowVictoryWidgetAbility : public UGameplayAbility
{
	GENERATED_BODY()


public:
	URS_ShowVictoryWidgetAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled) override;

	UFUNCTION()
	void OnVictoryWidgetFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URS_VictoryWidget> VictoryWidgetClass;

	UPROPERTY()
	TObjectPtr<URS_VictoryWidget> VictoryWidget;

	
};
