// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/CVWorldObject_Base.h"
#include "CVProjectile_Base.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


UCLASS()
class CHRONOVEIL_API ACVProjectile_Base : public ACVWorldObject_Base
{
	GENERATED_BODY()
	
public:
	ACVProjectile_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Projectile")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Projectile")
	float LifeTime = 5.f;

	FTimerHandle LifeHandle;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void HandleImpact(const FHitResult& Hit);
	virtual void HandleLifeTimeExpired();
};
