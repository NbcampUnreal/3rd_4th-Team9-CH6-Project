#include "Item/RSWeaponActor.h"

#include "Components/SkeletalMeshComponent.h"

ARSWeaponActor::ARSWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(Mesh);
}
