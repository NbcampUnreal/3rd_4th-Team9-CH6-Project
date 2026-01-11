#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/RSWorldObject_Base.h"
#include "RSProjectile_Base.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class REMNANTSOUL_API ARSProjectile_Base : public ARSWorldObject_Base
{
	GENERATED_BODY()

public:
	ARSProjectile_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Projectile")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Projectile")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Projectile")
	float LifeTime = 5.f;

	FTimerHandle LifeHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnProjectileHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	virtual void HandleImpact(const FHitResult& Hit);
	virtual void HandleLifeTimeExpired();
};
