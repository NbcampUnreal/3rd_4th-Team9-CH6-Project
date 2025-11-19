// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"
#include "CVProjectile_ClusterGrenade.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class CHRONOVEIL_API ACVProjectile_ClusterGrenade : public ACVProjectile_Base
{
    GENERATED_BODY()

public:
    ACVProjectile_ClusterGrenade();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* ExplosionRadiusComp;

    UPROPERTY(EditDefaultsOnly, Category = "FX")
    UNiagaraSystem* ExplosionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float FuseTime = 2.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float ExplosionDamage = 50.f;

    UPROPERTY(EditDefaultsOnly, Category = "Grenade")
    float ExplosionRadius = 400.f;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* BounceSound;

    FTimerHandle TimerHandle_Explode;

protected:
    virtual void BeginPlay() override;

    // Base의 OnProjectileHit를 override 해서 튕김/감속 처리
    virtual void OnProjectileHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit) override;

    // 폭발 처리 : 서버에서 처리
    void Explode();

    // 클라 전체에 폭발 FX 반영
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayExplosionFX();
};
