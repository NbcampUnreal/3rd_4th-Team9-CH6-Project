// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Weapon/CVWeaponRifle.h"

ACVWeaponRifle::ACVWeaponRifle()
{
	WeaponType = EWeaponType::Rifle;
	MaxAmmo = 30;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponRifle::Fire()
{
	ACVWeaponBase::Fire();
}

