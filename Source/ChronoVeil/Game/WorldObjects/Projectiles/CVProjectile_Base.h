// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/CVWorldObject_Base.h"
#include "CVProjectile_Base.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CHRONOVEIL_API ACVProjectile_Base : public ACVWorldObject_Base
{
    GENERATED_BODY()

public:
    ACVProjectile_Base();

protected:
    UPROPERTY(VisibleAnywhere, Category = "CV|Projectile")
    TObjectPtr<USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, Category = "CV|Projectile")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Projectile")
    TObjectPtr<UProjectileMovementComponent> Movement;

    // 기본 수명 (초). 0이면 무한.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Projectile")
    float LifeTime = 5.f;

    FTimerHandle LifeHandle;

    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnProjectileHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit);

    // 파생 클래스에서 충돌 처리용
    virtual void HandleImpact(const FHitResult& Hit);
    virtual void HandleLifeTimeExpired();
};
