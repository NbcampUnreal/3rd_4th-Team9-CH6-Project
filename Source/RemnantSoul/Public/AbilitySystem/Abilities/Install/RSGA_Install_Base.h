#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RSGameplayAbility.h"
#include "RSGA_Install_Base.generated.h"

struct FGameplayAbilityActorInfo;

UCLASS(Abstract)
class REMNANTSOUL_API URSGA_Install_Base : public URSGameplayAbility
{
	GENERATED_BODY()

public:
	URSGA_Install_Base();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
	TSubclassOf<AActor> DeviceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
	TArray<TSubclassOf<AActor>> DeviceClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
	FVector SpawnOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
	float LifeSpan = 0.f;

protected:
	AActor* SpawnDevice(
		TSubclassOf<AActor> InClass,
		const FTransform& SpawnTM,
		const FGameplayAbilityActorInfo* ActorInfo);

	AActor* SpawnSingleDevice(
		const FGameplayAbilityActorInfo* ActorInfo);

	TArray<AActor*> SpawnMultipleDevices(
		const FGameplayAbilityActorInfo* ActorInfo);
};
