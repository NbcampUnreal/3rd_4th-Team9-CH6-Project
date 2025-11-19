#include "Game/AbilitySystem/Abilities/Zone/CVGA_Zone_Base.h"
#include "Game/WorldObjects/Zones/CVZone_Base.h"
#include "GameFramework/Pawn.h"

UCVGA_Zone_Base::UCVGA_Zone_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SpawnOffset = FVector(200.f, 0.f, 0.f);
    LifeSpan = 0.f;
}

ACVZone_Base* UCVGA_Zone_Base::SpawnZone(
    TSubclassOf<ACVZone_Base> InClass,
    const FTransform& SpawnTM,
    const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnZone: Invalid input (Class or ActorInfo)"));
        return nullptr;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    UWorld* World = Avatar ? Avatar->GetWorld() : nullptr;
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnZone: World is NULL (Avatar=%s)"), *GetNameSafe(Avatar));
        return nullptr;
    }

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACVZone_Base* Zone = World->SpawnActor<ACVZone_Base>(
        InClass, SpawnTM, Params);

    if (!Zone)
    {
        return nullptr;
    }

    Zone->SetOwnerFromASC(Avatar);

    if (Zone)
    {
        Zone->SetOwnerFromASC(Avatar);

        if (LifeSpan > 0.f)
        {
            Zone->SetLifeSpan(LifeSpan);
        }
    }

    return Zone;
}