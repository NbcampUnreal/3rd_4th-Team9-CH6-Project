// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Weapon/CVWeaponBase.h"
#include "CVWeaponKnife.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOVEIL_API ACVWeaponKnife : public ACVWeaponBase
{
	GENERATED_BODY()

public:
	ACVWeaponKnife();

	virtual void Fire() override;
	
};
