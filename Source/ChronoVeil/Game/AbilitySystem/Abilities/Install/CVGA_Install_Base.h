// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Install_Base.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API UCVGA_Install_Base : public UCVGameplayAbility
{
	GENERATED_BODY()
	
public:
    UCVGA_Install_Base();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
    TSubclassOf<AActor> ObstacleClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
    TArray<TSubclassOf<AActor>> ObstacleClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    FVector SpawnOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    float LifeSpan = 0.f;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    TSubclassOf<class UCVGA_Install_Base> InstallClass;

    virtual void SpawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation);

};
