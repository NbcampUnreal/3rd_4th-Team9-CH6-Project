// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Weapon/CVWeaponSniper.h"

ACVWeaponSniper::ACVWeaponSniper()
{
	WeaponType = EWeaponType::AWP;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponSniper::Fire()
{
	// AWP heavy damage, single shot; reuse base
	ACVWeaponBase::Fire();
}