// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "CVDevice_ToyTower.generated.h"



UCLASS()
class CHRONOVEIL_API ACVDevice_ToyTower : public ACVDevice_Base
{
	GENERATED_BODY()

public:
	ACVDevice_ToyTower();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	FVector StartLocation;
	FVector TargetLocation;

	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RiseDuration = 0.5f;

	bool bIsMoving = true;

	void ResumeMovement();
};
