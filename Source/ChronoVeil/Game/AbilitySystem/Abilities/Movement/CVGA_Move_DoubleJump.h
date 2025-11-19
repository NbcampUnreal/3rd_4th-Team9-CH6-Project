#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"
#include "CVGA_Move_DoubleJump.generated.h"

UCLASS()
class CHRONOVEIL_API UCVGA_Move_DoubleJump : public UCVGA_Move_Base
{
    GENERATED_BODY()

public:
    UCVGA_Move_DoubleJump();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
    float JumpZVelocity = 750.f;

    virtual void PerformMove(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};
