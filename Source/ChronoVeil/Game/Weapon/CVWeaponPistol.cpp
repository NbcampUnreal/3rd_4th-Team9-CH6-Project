// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Weapon/CVWeaponPistol.h"

ACVWeaponPistol::ACVWeaponPistol()
{
	WeaponType = EWeaponType::Pistol;
	MaxAmmo = 12;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponPistol::Fire()
{
	// Pistol fires single shot; reuse base
	ACVWeaponBase::Fire();
}