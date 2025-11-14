// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Proj_Base.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API UCVGA_Proj_Base : public UCVGameplayAbility
{
	GENERATED_BODY()
	
public:
    UCVGA_Proj_Base();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
    TSubclassOf<AActor> ObstacleClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
    TArray<TSubclassOf<AActor>> ObstacleClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    FVector SpawnOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    float LifeSpan = 0.f;
};
