#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Flash.h"
#include "Game/Character/CVCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

UCVGA_Move_Flash::UCVGA_Move_Flash()
{
    // Flash도 LocalPredicted 가능하지만, 텔레포트 특성상 서버 우선으로 보는 경우도 있음.
}

void UCVGA_Move_Flash::PerformMove(
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

    UWorld* World = Character->GetWorld();
    if (!World)
    {
        return;
    }

    UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
    if (!Capsule)
    {
        return;
    }

    const FVector Forward = Character->GetActorForwardVector().GetSafeNormal2D();
    if (Forward.IsNearlyZero())
    {
        return;
    }

    const FVector Start = Character->GetActorLocation();
    const FVector End = Start + Forward * MaxFlashDistance;

    const float CapsuleRadius = Capsule->GetScaledCapsuleRadius() + FlashTraceRadius;
    const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

    FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(FlashTrace), false, Character);
    FHitResult Hit;

    bool bHit = World->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        CapsuleShape,
        Params
    );

    FVector TargetLocation = End;

    if (bHit)
    {
        // 벽 앞쪽으로 한 칸 물려서 텔레포트
        TargetLocation = Hit.Location - Forward * WallSafeDistance;
    }

    // 바닥 보정이 필요하면 여기서 TraceDown 해서 높이 맞추는 것도 가능
    Character->TeleportTo(TargetLocation, Character->GetActorRotation(), false, true);
}
