// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_Base.h"
#include "CVGA_Proj_ClusterGrenade.generated.h"

class ACVProjectile_ClusterGrenade;
class UNiagaraSystem;
class USphereComponent;

UCLASS()
class CHRONOVEIL_API UCVGA_Proj_ClusterGrenade : public UCVGA_Proj_Base
{
	GENERATED_BODY()
	
public:
    UCVGA_Proj_ClusterGrenade();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    TSubclassOf<ACVProjectile_ClusterGrenade> GrenadeClass;

    // 던지는 힘 (거리에 영향)
    UPROPERTY(EditDefaultsOnly, Category = "Grenade|Throw")
    float ThrowStrength = 1500.f;

    // 던질 때 약간 위로 향하도록 각도 보정
    UPROPERTY(EditDefaultsOnly, Category = "Grenade|Throw")
    float ThrowPitchAngle = 15.f;   // 15도 위로 던지기

    //// 캐릭터 기준 생성 위치 : 앞이랑 위쪽에서 생성함.
    //UPROPERTY(EditDefaultsOnly, Category = "Grenade|Spawn")
    //FVector SpawnOffset = FVector(70.f, 0.f, 50.f);

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

};
