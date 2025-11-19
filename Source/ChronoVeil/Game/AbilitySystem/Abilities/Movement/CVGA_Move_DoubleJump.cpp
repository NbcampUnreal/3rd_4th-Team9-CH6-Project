#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_DoubleJump.h"
#include "Game/Character/CVCharacter.h"

UCVGA_Move_DoubleJump::UCVGA_Move_DoubleJump()
{
}

void UCVGA_Move_DoubleJump::PerformMove(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (ACVCharacter* Character = GetCVCharacter(ActorInfo))
    {
        Character->LaunchCharacter(FVector(0.f, 0.f, JumpZVelocity), false, true);
    }
}
