#pragma once

#include "RSDefine.h"
#include "Components/PawnComponent.h"
#include "RSCosmeticManagerComponent.generated.h"

class ARSArmorBase;
class URSItemFragment_Equipable_Armor;

UCLASS(BlueprintType, Blueprintable)
class URSCosmeticManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	URSCosmeticManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	void RefreshArmorMesh(EArmorType ArmorType, const URSItemFragment_Equipable_Armor* ArmorFragment);
	void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const;

private:
	void InitializeManager();
	
	UChildActorComponent* SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial);
	void SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	ECharacterSkinType CharacterSkinType = ECharacterSkinType::Boy;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARSArmorBase> CosmeticSlotClass;

private:
	UPROPERTY()
	TObjectPtr<UChildActorComponent> HeadSlot;
	
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
};
