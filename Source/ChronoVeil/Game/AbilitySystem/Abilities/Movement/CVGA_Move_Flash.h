#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_Flash.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_Flash : public UCVGA_Move_Base
{
	GENERATED_BODY()

public:
    UCVGA_Move_Flash();

protected:
    // 최대 플래시 거리
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash")
    float MaxFlashDistance = 800.f;

    // 충돌 감지용 캡슐 반지름 보정
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash")
    float FlashTraceRadius = 10.f;

    // 벽 바로 앞에 서기 위한 여유 거리
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flash")
    float WallSafeDistance = 10.f;

protected:
    virtual void PerformMove(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};
