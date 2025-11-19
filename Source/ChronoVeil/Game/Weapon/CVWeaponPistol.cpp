#include "Game/Weapon/CVWeaponPistol.h"

ACVWeaponPistol::ACVWeaponPistol()
{
	WeaponType = EWeaponType::Pistol;
	MaxAmmo = 12;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponPistol::Fire()
{
	ACVWeaponBase::Fire();
}