#pragma once

#include "CoreMinimal.h"
#include "Game/Weapon/CVWeaponBase.h"
#include "CVWeaponRifle.generated.h"

UCLASS()
class CHRONOVEIL_API ACVWeaponRifle : public ACVWeaponBase
{
	GENERATED_BODY()
	
public:
	ACVWeaponRifle();

	virtual void Fire() override;
};
