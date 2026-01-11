#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RSGameplayAbility.h"
#include "RSGA_Zone_Base.generated.h"

struct FGameplayAbilityActorInfo;

UCLASS(Abstract)
class REMNANTSOUL_API URSGA_Zone_Base : public URSGameplayAbility
{
	GENERATED_BODY()

public:
	URSGA_Zone_Base();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone")
	TSubclassOf<AActor> ZoneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone")
	TArray<TSubclassOf<AActor>> ZoneClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	FVector SpawnOffset = FVector(200.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	float LifeSpan = 0.f;

protected:
	AActor* SpawnZone(
		TSubclassOf<AActor> InClass,
		const FTransform& SpawnTM,
		const FGameplayAbilityActorInfo* ActorInfo
	);

	AActor* SpawnSingleZone(const FGameplayAbilityActorInfo* ActorInfo);
	TArray<AActor*> SpawnMultipleZones(const FGameplayAbilityActorInfo* ActorInfo);
};
