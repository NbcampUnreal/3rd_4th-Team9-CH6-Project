// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "CVGA_Install_HpPotion.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API UCVGA_Install_HpPotion : public UCVGA_Install_Base
{
	GENERATED_BODY()
	
public:
    UCVGA_Install_HpPotion();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
};
