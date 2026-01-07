#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "RSCosmeticManagerComponent.generated.h"

class ARSCharacter;
class ARSWeaponActor;
class URSItemInstance;
class URSItemTemplate;
class URSItemFragment_WeaponCosmetic;

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSCosmeticManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSCosmeticManagerComponent();

protected:
	virtual void BeginPlay() override;

	void CacheOwnerCharacter();

public:
	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void ApplyWeaponFromItem(URSItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void ClearWeapon();

protected:
	void SpawnAndAttachWeaponActor(const URSItemTemplate* ItemTemplate, const URSItemFragment_WeaponCosmetic* CosFrag);
	void DestroyCurrentWeaponActor();

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<ARSCharacter> CachedCharacter;

	UPROPERTY(Transient)
	TObjectPtr<ARSWeaponActor> CurrentWeaponActor;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	FName DefaultWeaponAttachSocketName = TEXT("hand_rSocket");

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	bool bDestroyWeaponOnUnequip = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RS|Cosmetic")
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

#pragma region Holster

public:
	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void SyncWeaponCosmetics(URSItemInstance* MainItem, URSItemInstance* SubItem);

	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void ClearAllWeapons();

private:
	void ApplyWeaponToSlot(bool bIsMain, URSItemInstance* ItemInstance);
	void DestroyWeaponActor(bool bIsMain);
	ARSWeaponActor* SpawnWeaponActor(const URSItemTemplate* ItemTemplate, const URSItemFragment_WeaponCosmetic* CosFrag);

	FName ResolveEquippedSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const;
	FName ResolveHolsterSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const;

private:
	UPROPERTY(Transient) TObjectPtr<ARSWeaponActor> MainWeaponActor = nullptr;
	UPROPERTY(Transient) TObjectPtr<ARSWeaponActor> SubWeaponActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	FName DefaultEquippedSocketName = TEXT("hand_rSocket");

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	FName DefaultHolsterSocketName = TEXT("spine_03Socket");

#pragma endregion
};
