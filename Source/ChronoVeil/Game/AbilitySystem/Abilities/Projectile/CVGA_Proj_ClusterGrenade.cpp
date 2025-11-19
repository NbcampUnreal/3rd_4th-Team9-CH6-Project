#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_ClusterGrenade.h"
#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

UCVGA_Proj_ClusterGrenade::UCVGA_Proj_ClusterGrenade()
{
    SpawnOffset = FVector(70.f, 0.f, 50.f);
}

void UCVGA_Proj_ClusterGrenade::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!HasAuthority(ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !ProjectileClass)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    UWorld* World = Avatar->GetWorld();
    if (!World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FVector SpawnLoc =
        Avatar->GetActorLocation()
        + Avatar->GetActorForwardVector() * SpawnOffset.X
        + FVector(0.f, 0.f, SpawnOffset.Z);

    const FRotator SpawnRot = Avatar->GetActorRotation();
    const FTransform SpawnTM(SpawnRot, SpawnLoc);

    ACVProjectile_Base* Grenade = SpawnProjectile(ProjectileClass, SpawnTM, ActorInfo);

    if (Grenade)
    {
        // 던질 방향 계산 : Forward + 위쪽으로 약간 각도
        const FVector Forward = Avatar->GetActorForwardVector();
        const FVector Up = Avatar->GetActorUpVector();

        const float PitchRad = FMath::DegreesToRadians(ThrowPitchAngle);
        FVector ThrowDir = (Forward + Up * FMath::Tan(PitchRad)).GetSafeNormal();

        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Grenade->GetRootComponent()))
        {
            // 중력에 영향받으면 던져짐.
            RootComp->AddImpulse(ThrowDir * ThrowStrength, NAME_None, true);
        }
    }

    // 한 번 던지면 끝나는 단발형 스킬임.
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
