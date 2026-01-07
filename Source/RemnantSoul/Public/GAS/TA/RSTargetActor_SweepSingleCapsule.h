// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityTargetActor.h"
#include "RSTargetActor_SweepSingleCapsule.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API ARSTargetActor_SweepSingleCapsule : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ARSTargetActor_SweepSingleCapsule();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;
};
