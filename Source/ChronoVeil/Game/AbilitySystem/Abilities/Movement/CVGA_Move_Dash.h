#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_Dash.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_Dash : public UCVGA_Move_Base
{
	GENERATED_BODY()

protected:
	// 대시 세기
	UPROPERTY(EditDefaultsOnly, Category = "Move|Dash")
	float DashStrength = 1200.f;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};