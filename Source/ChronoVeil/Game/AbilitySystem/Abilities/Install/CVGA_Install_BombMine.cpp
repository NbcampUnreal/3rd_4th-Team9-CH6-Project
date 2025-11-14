// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_BombMine.h"
#include "Game/WorldObjects/Devices/CVDevice_BombMine.h"

UCVGA_Install_BombMine::UCVGA_Install_BombMine()
{
    InstallClass = ACVDevice_BombMine::StaticClass();
}

void UCVGA_Install_BombMine::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        AActor* Avatar = ActorInfo->AvatarActor.Get();
        if (Avatar && InstallClass)
        {
            FVector SpawnLoc = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 200.f;
            FRotator SpawnRot = Avatar->GetActorRotation();

            SpawnItem(SpawnLoc, SpawnRot);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}