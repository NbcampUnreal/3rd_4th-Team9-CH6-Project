#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/CVWorldObject_Base.h"
#include "CVDevice_Base.generated.h"

//class UHealthComponent; // 필요하면 체력 컴포넌트 포함
class USphereComponent;

UCLASS()
class CHRONOVEIL_API ACVDevice_Base : public ACVWorldObject_Base
{
	GENERATED_BODY()
	
public:
	ACVDevice_Base();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Device")
	float LifeTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CV|Device")
	bool bDestroyOnOwnerDeath = false;

	FTimerHandle LifeTimerHandle;

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
	// 필요하면 체력 컴포넌트/인터페이스 연결
	virtual void OnDamaged(float Damage, AActor* DamageCauser);

};
