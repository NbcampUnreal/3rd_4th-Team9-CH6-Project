#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/RSWorldObject_Base.h"
#include "RSDevice_Base.generated.h"

class USphereComponent;

UCLASS()
class REMNANTSOUL_API ARSDevice_Base : public ARSWorldObject_Base
{
	GENERATED_BODY()

public:
	ARSDevice_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Device")
	float LifeTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Device")
	bool bDestroyOnOwnerDeath = false;

	FTimerHandle LifeTimerHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleLifeTimeExpired();

	UFUNCTION()
	virtual void OnDeviceOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:
	virtual void OnDamaged(float Damage, AActor* DamageCauser);
};
