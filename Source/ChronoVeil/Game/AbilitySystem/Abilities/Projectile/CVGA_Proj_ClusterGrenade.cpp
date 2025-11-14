// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_ClusterGrenade.h"
#include "Game/WorldObjects/Projectiles/CVProjectile_ClusterGrenade.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

UCVGA_Proj_ClusterGrenade::UCVGA_Proj_ClusterGrenade()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

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

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (!Avatar || !GrenadeClass)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FVector SpawnLoc = Avatar->GetActorLocation()
        + Avatar->GetActorForwardVector() * SpawnOffset.X
        + FVector(0, 0, SpawnOffset.Z);

    FRotator SpawnRot = Avatar->GetActorRotation();

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACVProjectile_Base* Grenade = Avatar->GetWorld()->SpawnActor<ACVProjectile_Base>(
        GrenadeClass, SpawnLoc, SpawnRot, Params);

    if (Grenade)
    {
        //  던지는 방향 계산
        FVector Forward = Avatar->GetActorForwardVector();
        FVector Up = Avatar->GetActorUpVector();

        // 위쪽으로 ThrowPitchAngle도만큼 던지기 (15도 정도)
        FVector ThrowDir = (Forward + Up * FMath::Tan(FMath::DegreesToRadians(ThrowPitchAngle))).GetSafeNormal();

        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Grenade->GetRootComponent()))
        {
            //  던지기 힘 적용 : 중력에 영향받음. 
            RootComp->AddImpulse(ThrowDir * ThrowStrength, NAME_None, true);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
