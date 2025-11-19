#pragma once

#include "CoreMinimal.h"
#include "Game/Weapon/CVWeaponBase.h"
#include "CVWeaponPistol.generated.h"

UCLASS()
class CHRONOVEIL_API ACVWeaponPistol : public ACVWeaponBase
{
	GENERATED_BODY()

public:
	ACVWeaponPistol();

	virtual void Fire() override;
};