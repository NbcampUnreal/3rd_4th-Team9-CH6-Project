#include "Game/Weapon/CVWeaponSniper.h"

ACVWeaponSniper::ACVWeaponSniper()
{
	WeaponType = EWeaponType::AWP;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponSniper::Fire()
{
	ACVWeaponBase::Fire();
}