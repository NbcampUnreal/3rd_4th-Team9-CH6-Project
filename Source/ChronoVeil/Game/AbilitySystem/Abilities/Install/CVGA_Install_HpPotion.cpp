// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_HpPotion.h"
#include "Game/WorldObjects/Devices/CVDevice_HpPotion.h"


UCVGA_Install_HpPotion::UCVGA_Install_HpPotion()
{
    InstallClass = ACVDevice_HpPotion::StaticClass();
}

void UCVGA_Install_HpPotion::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        AActor* Avatar = ActorInfo->AvatarActor.Get();
        if (Avatar)
        {
            FVector SpawnLoc = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 200.f;
            FRotator SpawnRot = Avatar->GetActorRotation();

            SpawnItem(SpawnLoc, SpawnRot);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}