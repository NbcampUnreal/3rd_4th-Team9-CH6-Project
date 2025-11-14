// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Weapon/CVWeaponBase.h"
#include "CVWeaponSniper.generated.h"

UCLASS()
class CHRONOVEIL_API ACVWeaponSniper : public ACVWeaponBase
{
	GENERATED_BODY()

public:
	ACVWeaponSniper();

	virtual void Fire() override;
};
