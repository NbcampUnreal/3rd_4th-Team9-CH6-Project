#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_Flash.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_Flash : public UCVGA_Move_Base
{
	GENERATED_BODY()

protected:
	// 최대 점멸 거리
	UPROPERTY(EditDefaultsOnly, Category = "Move|Flash")
	float MaxFlashDistance = 800.f;

	// 바닥에서 약간 띄워서 텔레포트할 오프셋
	UPROPERTY(EditDefaultsOnly, Category = "Move|Flash")
	float TeleportHeightOffset = 0.f;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
