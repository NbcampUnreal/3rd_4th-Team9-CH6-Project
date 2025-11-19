#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"

#include "CVWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Knife,
	Pistol,
	Rifle,
	AWP
};

class USkeletalMeshComponent;

UCLASS()
class CHRONOVEIL_API ACVWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ACVWeaponBase();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void OnRep_CurrentAmmo();

	// Fire the weapon (calls server when needed)
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayFire();

	// Pickup handling
	UFUNCTION()
	void OnPickupAreaBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USphereComponent* PickupArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;
};








