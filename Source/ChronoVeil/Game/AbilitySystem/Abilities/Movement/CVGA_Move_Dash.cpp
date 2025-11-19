#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Dash.h"
#include "Game/Character/CVCharacter.h"
#include "Game/Player/CVPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

UCVGA_Move_Dash::UCVGA_Move_Dash()
{
    // 필요하면 여기서 NetExecutionPolicy/InstancingPolicy 조정 가능
}

void UCVGA_Move_Dash::PerformMove(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    ACVCharacter* Character = GetCVCharacter(ActorInfo);
    if (!Character)
    {
        return;
    }

    FVector DashDir = Character->GetActorForwardVector();

    // 입력 방향 사용 옵션
    if (bUseInputDirection)
    {
        if (ACVPlayerController* PC = Cast<ACVPlayerController>(ActorInfo->PlayerController.Get()))
        {
            // 여기서 실제 입력 벡터를 가져오는 방식은 프로젝트에 따라 다를 수 있음 (확실하지 않음)
            // 예시로 CharacterMovement의 LastInputVector를 사용
            const FVector InputDir = Character->GetLastMovementInputVector();
            if (!InputDir.IsNearlyZero())
            {
                DashDir = InputDir.GetSafeNormal2D();
            }
        }
    }

    DashDir = DashDir.GetSafeNormal2D();
    if (DashDir.IsNearlyZero())
    {
        return;
    }

    const FVector LaunchVelocity = DashDir * DashStrength;

    // Z는 유지하고 XY만 Launch - 지면 대시 느낌
    Character->LaunchCharacter(
        FVector(LaunchVelocity.X, LaunchVelocity.Y, 0.0f),
        true,   // XY Override
        false   // Z Override
    );
}
