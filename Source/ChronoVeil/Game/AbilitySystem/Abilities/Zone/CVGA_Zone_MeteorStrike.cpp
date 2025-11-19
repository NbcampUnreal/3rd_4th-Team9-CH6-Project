#include "Game/AbilitySystem/Abilities/Zone/CVGA_Zone_MeteorStrike.h"
#include "Game/WorldObjects/Zones/CVZone_MeteorImpact.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

UCVGA_Zone_MeteorStrike::UCVGA_Zone_MeteorStrike()
{
    SpawnOffset = FVector(500.f, 0.f, 0.f);
}

void UCVGA_Zone_MeteorStrike::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    if (!HasAuthority(ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !MeteorImpactZoneClass)
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
        Avatar->GetActorLocation() +
        Avatar->GetActorRotation().RotateVector(SpawnOffset);

    const FRotator SpawnRot = FRotator::ZeroRotator;
    const FTransform SpawnTM(SpawnRot, SpawnLoc);

    ACVZone_Base* MeteorZone = SpawnZone(MeteorImpactZoneClass, SpawnTM, ActorInfo);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

