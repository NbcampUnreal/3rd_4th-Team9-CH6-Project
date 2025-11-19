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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* BoxCollision;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "JumpPad")
    float LaunchVelocity = 1200.f;

protected:
    UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );
};
