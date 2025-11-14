// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"
#include "CVProjectile_ClusterGrenade.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CHRONOVEIL_API ACVProjectile_ClusterGrenade : public ACVProjectile_Base
{
	GENERATED_BODY()
	
public:
    ACVProjectile_ClusterGrenade();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

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
    UFUNCTION()
    void OnGrenadeHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 폭발 처리 : 서버에서 처리함.
    void Explode();

    // 클라 전체에 폭발 FX 반영
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayExplosionFX();
};
