#include "Game/AbilitySystem/Abilities/Zone/CVGA_Zone_MeteorStrike.h"
#include "Game/WorldObjects/Zones/CVZone_MeteorImpact.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UCVGA_Zone_MeteorStrike::UCVGA_Zone_MeteorStrike()
{
    SpawnOffset = FVector(200.f, 0.f, 0.f);
}


void UCVGA_Zone_MeteorStrike::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    UWorld* World = Avatar->GetWorld();
    if (!World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FVector ForwardAdjustedOffset = Avatar->GetActorRotation().RotateVector(SpawnOffset);
    FVector SpawnLoc = Avatar->GetActorLocation()
        + Avatar->GetActorForwardVector() * 500.f;
    FRotator SpawnRot = FRotator::ZeroRotator;

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACVZone_MeteorImpact* Meteor = World->SpawnActor<ACVZone_MeteorImpact>(MeteorImpactZoneClass, SpawnLoc, SpawnRot, Params);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


