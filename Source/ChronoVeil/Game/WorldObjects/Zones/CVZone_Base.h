// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/CVWorldObject_Base.h"
#include "CVZone_Base.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API ACVZone_Base : public ACVWorldObject_Base
{
	GENERATED_BODY()
	
public:
	ACVZone_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Zone")
	TObjectPtr<UShapeComponent> ZoneShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Zone")
	float LifeTime = 0.f; // 0 = infinite

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Zone")
	float TickInterval = 0.2f;

	FTimerHandle LifeHandle;
	FTimerHandle TickHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void ZoneTick();

	UFUNCTION()
	virtual void HandleLifeTimeExpired();
};
