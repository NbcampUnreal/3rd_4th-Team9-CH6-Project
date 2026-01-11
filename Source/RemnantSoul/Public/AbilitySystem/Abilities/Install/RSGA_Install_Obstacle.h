#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Install/RSGA_Install_Base.h"
#include "RSGA_Install_Obstacle.generated.h"

UCLASS()
class REMNANTSOUL_API URSGA_Install_Obstacle : public URSGA_Install_Base
{
	GENERATED_BODY()

public:
	URSGA_Install_Obstacle();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|Obstacle")
	TSubclassOf<AActor> ObstacleClass;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
