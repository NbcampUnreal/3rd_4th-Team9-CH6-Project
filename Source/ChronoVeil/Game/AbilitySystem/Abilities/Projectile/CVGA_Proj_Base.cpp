#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_Base.h"
#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"   
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

UCVGA_Proj_Base::UCVGA_Proj_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SpawnOffset = FVector(200.f, 0.f, 0.f);
}

ACVProjectile_Base* UCVGA_Proj_Base::SpawnProjectile(
    TSubclassOf<ACVProjectile_Base> InClass,
    const FTransform& SpawnTM,
    const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACVProjectile_Base* Projectile = World->SpawnActor<ACVProjectile_Base>(
        InClass, SpawnTM, Params);

    if (Projectile)
    {
        Projectile->SetOwnerFromASC(Avatar);

        if (LifeSpan > 0.f)
        {
            Projectile->SetLifeSpan(LifeSpan);
        }
    }

    return Projectile;
}
