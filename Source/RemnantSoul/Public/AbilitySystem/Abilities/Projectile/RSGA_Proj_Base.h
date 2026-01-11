#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RSGameplayAbility.h"
#include "RSGA_Proj_Base.generated.h"

class AActor;
struct FGameplayAbilityActorInfo;

UCLASS(Abstract)
class REMNANTSOUL_API URSGA_Proj_Base : public URSGameplayAbility
{
	GENERATED_BODY()

public:
	URSGA_Proj_Base();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TArray<TSubclassOf<AActor>> ProjectileClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	FVector SpawnOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	float LifeSpan = 0.f;

protected:
	AActor* SpawnProjectile(
		TSubclassOf<AActor> InClass,
		const FTransform& SpawnTM,
		const FGameplayAbilityActorInfo* ActorInfo
	);
};
