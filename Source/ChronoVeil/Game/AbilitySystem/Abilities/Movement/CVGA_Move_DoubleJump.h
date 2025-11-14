// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_DoubleJump.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_DoubleJump : public UCVGA_Move_Base
{
	GENERATED_BODY()

protected:
	// Á¡ÇÁ Èû
	UPROPERTY(EditDefaultsOnly, Category = "Move|DoubleJump")
	float JumpStrength = 750.f;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
