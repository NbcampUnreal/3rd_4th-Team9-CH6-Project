#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Move_Base.generated.h"

UCLASS(Abstract)
class CHRONOVEIL_API UCVGA_Move_Base : public UCVGameplayAbility
{
	GENERATED_BODY()

public:
    UCVGA_Move_Base();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

protected:
    // 실제 이동 로직은 여기서 구현하게 유도
    virtual void PerformMove(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData);
};
