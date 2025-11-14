// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "CVDevice_JumpPad.generated.h"

class UBoxComponent;

UCLASS()
class CHRONOVEIL_API ACVDevice_JumpPad : public ACVDevice_Base
{
	GENERATED_BODY()

public:
    ACVDevice_JumpPad();


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;


protected:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpPad")
    float LaunchStrength = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float GrowDuration;

    bool bGrowing;
    float StartTime;



    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

};
