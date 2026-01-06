#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSWeaponPickupActor.generated.h"

class URSItemTemplate;
class USphereComponent;
class USkeletalMeshComponent;

UCLASS(Blueprintable)
class REMNANTSOUL_API ARSWeaponPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ARSWeaponPickupActor();


	UFUNCTION(BlueprintPure, Category = "RS|Pickup")
	const URSItemTemplate* GetPickupTemplate() const { return PickupItemTemplate.Get(); }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USkeletalMeshComponent> VisualMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSItemTemplate> PickupItemTemplate;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "RS|Pickup")
	int32 PickupCount = 1;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "RS|Pickup")
	bool bAutoEquip = true;

	UPROPERTY(Transient)
	bool bConsumed = false;
};
