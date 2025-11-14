// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Weapon/CVWeaponKnife.h"
#include "Kismet/GameplayStatics.h"

ACVWeaponKnife::ACVWeaponKnife()
{
	WeaponType = EWeaponType::Knife;
	MaxAmmo = 0;
	CurrentAmmo = 0;
}

void ACVWeaponKnife::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
		return;
	}

	MulticastPlayFire();
}

