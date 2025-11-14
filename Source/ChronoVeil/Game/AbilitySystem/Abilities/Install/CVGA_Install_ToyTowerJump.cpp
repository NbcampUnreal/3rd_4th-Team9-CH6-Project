// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_ToyTowerJump.h"
#include "Game/WorldObjects/Devices/CVDevice_JumpPad.h"
#include "Game/WorldObjects/Devices/CVDevice_ToyTower.h"
#include "Game/Character/CVCharacter.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UCVGA_Install_ToyTowerJump::UCVGA_Install_ToyTowerJump()
{
    SpawnOffset = FVector(1200.0f, 0.f, 0.f);
    LifeSpan = 10.f;
}

void UCVGA_Install_ToyTowerJump::ActivateAbility(
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

    SpawnObstacleActors(ActorInfo);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}



void UCVGA_Install_ToyTowerJump::SpawnObstacleActors(const FGameplayAbilityActorInfo* ActorInfo)
{
    if (ObstacleClasses.Num() == 0 || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    UWorld* World = Avatar->GetWorld();
    if (!World)
    {
        return;
    }

    for (TSubclassOf<AActor> ObstacleClassItem : ObstacleClasses)
    {
        if (!ObstacleClassItem)
        {
            continue;
        }

        FVector SpawnLocation =
            Avatar->GetActorLocation() +
            Avatar->GetActorForwardVector() * SpawnOffset.X;

        SpawnLocation.Z += SpawnOffset.Z;

        if (ObstacleClassItem->IsChildOf(ACVDevice_JumpPad::StaticClass()))
        {
            SpawnLocation += Avatar->GetActorForwardVector() * -500.0f;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Avatar;
        SpawnParams.Instigator = Cast<APawn>(Avatar);
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        if (AActor* Obstacle = World->SpawnActor<AActor>(ObstacleClassItem, SpawnLocation, Avatar->GetActorRotation(), SpawnParams))
        {
            // 일정시간 뒤 삭제
            //if (LifeSpan > 0.f) 
            //{
            //    Obstacle->SetLifeSpan(LifeSpan);
            //}
        }

    }
}
