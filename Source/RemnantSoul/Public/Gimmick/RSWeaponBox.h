#pragma once

#include "RSItemBox.h"
#include "RSWeaponBox.generated.h"

UCLASS()
class REMNANTSOUL_API ARSWeaponBox : public ARSItemBox
{
	GENERATED_BODY()

protected:
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag WeaponEventTag;

};
