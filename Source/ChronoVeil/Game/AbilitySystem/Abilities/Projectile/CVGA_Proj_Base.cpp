// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_Base.h"

UCVGA_Proj_Base::UCVGA_Proj_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SpawnOffset = FVector(200.f, 0.f, 0.f);
}