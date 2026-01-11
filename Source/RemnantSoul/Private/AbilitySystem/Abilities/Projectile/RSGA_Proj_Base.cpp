#include "AbilitySystem/Abilities/Projectile/RSGA_Proj_Base.h"

#include "GameFramework/Pawn.h"
#include "Engine/World.h"

URSGA_Proj_Base::URSGA_Proj_Base()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
}

AActor* URSGA_Proj_Base::SpawnProjectile(
	TSubclassOf<AActor> InClass,
	const FTransform& SpawnTM,
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Cast<APawn>(Avatar);
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Projectile = World->SpawnActor<AActor>(InClass, SpawnTM, Params);
	if (Projectile && LifeSpan > 0.f)
	{
		Projectile->SetLifeSpan(LifeSpan);
	}

	return Projectile;
}
