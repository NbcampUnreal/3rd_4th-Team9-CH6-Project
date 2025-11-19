#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_Dash.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_Dash : public UCVGA_Move_Base
{
    GENERATED_BODY()

public:
    UCVGA_Move_Dash();

protected:
    // 지면 기준 대시 속도 (LaunchCharacter에 들어갈 값)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
    float DashStrength = 2000.f;

    // 대시 방향을 순수 Forward만 쓸지, 입력 벡터를 섞을지 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
    bool bUseInputDirection = true;

protected:
    virtual void PerformMove(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};