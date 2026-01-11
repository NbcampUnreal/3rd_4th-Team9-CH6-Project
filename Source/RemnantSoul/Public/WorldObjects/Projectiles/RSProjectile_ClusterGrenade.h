#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/Projectiles/RSProjectile_Base.h"
#include "RSProjectile_ClusterGrenade.generated.h"

class UNiagaraSystem;
class USphereComponent;
class USoundBase;

UCLASS()
class REMNANTSOUL_API ARSProjectile_ClusterGrenade : public ARSProjectile_Base
{
	GENERATED_BODY()

public:
	ARSProjectile_ClusterGrenade();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> ExplosionRadiusComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	TObjectPtr<UNiagaraSystem> ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	float FuseTime = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	float ExplosionDamage = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	float ExplosionRadius = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> BounceSound;

	FTimerHandle TimerHandle_Explode;

protected:
	virtual void BeginPlay() override;

	virtual void OnProjectileHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit) override;

	void Explode();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionFX();
};
