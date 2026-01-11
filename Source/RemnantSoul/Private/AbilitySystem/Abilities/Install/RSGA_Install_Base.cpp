#include "AbilitySystem/Abilities/Install/RSGA_Install_Base.h"

#include "GameFramework/Pawn.h"
#include "Engine/World.h"

URSGA_Install_Base::URSGA_Install_Base()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
	LifeSpan = 0.f;
}

AActor* URSGA_Install_Base::SpawnDevice(
	TSubclassOf<AActor> InClass,
	const FTransform& SpawnTM,
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Cast<APawn>(Avatar);
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Device = World->SpawnActor<AActor>(InClass, SpawnTM, Params);
	if (Device && LifeSpan > 0.f)
	{
		Device->SetLifeSpan(LifeSpan);
	}

	return Device;
}

AActor* URSGA_Install_Base::SpawnSingleDevice(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!DeviceClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	FVector Loc = Avatar->GetActorLocation()
		+ Avatar->GetActorForwardVector() * SpawnOffset.X;
	Loc.Z += SpawnOffset.Z;

	const FRotator Rot = Avatar->GetActorRotation();
	const FTransform SpawnTM(Rot, Loc);

	return SpawnDevice(DeviceClass, SpawnTM, ActorInfo);
}

TArray<AActor*> URSGA_Install_Base::SpawnMultipleDevices(const FGameplayAbilityActorInfo* ActorInfo)
{
	TArray<AActor*> OutDevices;

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return OutDevices;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	for (const TSubclassOf<AActor>& ClassItem : DeviceClasses)
	{
		if (!*ClassItem)
		{
			continue;
		}

		FVector Loc = Avatar->GetActorLocation()
			+ Avatar->GetActorForwardVector() * SpawnOffset.X;
		Loc.Z += SpawnOffset.Z;

		const FRotator Rot = Avatar->GetActorRotation();
		const FTransform SpawnTM(Rot, Loc);

		if (AActor* Device = SpawnDevice(ClassItem, SpawnTM, ActorInfo))
		{
			OutDevices.Add(Device);
		}
	}

	return OutDevices;
}
