#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "CVDevice_HpPotion.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API ACVDevice_HpPotion : public ACVDevice_Base
{
	GENERATED_BODY()
	
protected:
	virtual void OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;
};
