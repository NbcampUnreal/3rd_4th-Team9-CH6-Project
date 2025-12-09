#pragma once

#include "RSDefine.h"
#include "RSClassData.generated.h"

class URSItemTemplate;
class ULyraAbilitySet;

USTRUCT(BlueprintType)
struct FRSDefaultItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSItemTemplate> ItemTemplateClass;

	UPROPERTY(EditDefaultsOnly)
	EItemRarity ItemRarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FRSClassInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FRSDefaultItemEntry> DefaultItemEntries;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULyraAbilitySet> ClassAbilitySet;
};

UCLASS(BlueprintType, Const)
class URSClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const URSClassData& Get();

public:
	const FRSClassInfoEntry& GetClassInfoEntry(ECharacterClassType ClassType) const;
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeNum="ECharacterClassType"))
	FRSClassInfoEntry ClassInfoEntries[(int32)ECharacterClassType::Count];
};
