#include "AbilitySystem/Abilities/Zone/RSGA_Zone_Base.h"

#include "GameFramework/Pawn.h"
#include "Engine/World.h"

URSGA_Zone_Base::URSGA_Zone_Base()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
	LifeSpan = 0.f;
}

AActor* URSGA_Zone_Base::SpawnZone(
	TSubclassOf<AActor> InClass,
	const FTransform& SpawnTM,
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	UWorld* World = Avatar ? Avatar->GetWorld() : nullptr;
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Cast<APawn>(Avatar);
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Zone = World->SpawnActor<AActor>(InClass, SpawnTM, Params);
	if (Zone && LifeSpan > 0.f)
	{
		Zone->SetLifeSpan(LifeSpan);
	}

	return Zone;
}

AActor* URSGA_Zone_Base::SpawnSingleZone(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!ZoneClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	const FVector SpawnLoc =
		Avatar->GetActorLocation() +
		Avatar->GetActorRotation().RotateVector(SpawnOffset);

	const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);
	return SpawnZone(ZoneClass, SpawnTM, ActorInfo);
}

TArray<AActor*> URSGA_Zone_Base::SpawnMultipleZones(const FGameplayAbilityActorInfo* ActorInfo)
{
	TArray<AActor*> OutZones;

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return OutZones;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	for (const TSubclassOf<AActor>& ClassItem : ZoneClasses)
	{
		if (!*ClassItem)
		{
			continue;
		}

		const FVector SpawnLoc =
			Avatar->GetActorLocation() +
			Avatar->GetActorRotation().RotateVector(SpawnOffset);

		const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);

		if (AActor* Zone = SpawnZone(ClassItem, SpawnTM, ActorInfo))
		{
			OutZones.Add(Zone);
		}
	}

	return OutZones;
}
