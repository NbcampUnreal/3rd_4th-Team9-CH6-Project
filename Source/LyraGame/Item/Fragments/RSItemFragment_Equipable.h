#pragma once

#include "RSDefine.h"
#include "GameplayTags.h"
#include "Item/RSItemTemplate.h"
#include "RSItemFragment_Equipable.generated.h"

class ULyraAbilitySet;
class URSItemInstance;

USTRUCT(BlueprintType)
struct FRarityStat
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatSet
{
	GENERATED_BODY()

public:
	FRarityStatSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStat> RarityStats;
};

USTRUCT(BlueprintType)
struct FRarityStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatRangeSet
{
	GENERATED_BODY()

public:
	FRarityStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStatRange> RarityStatRanges;
};

UCLASS(Abstract, Const)
class URSItemFragment_Equipable : public URSItemFragment
{
	GENERATED_BODY()
	
public:
	URSItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	void AddStatTagStack(URSItemInstance* ItemInstance, const TArray<FRarityStatSet>& RarityStatSets) const;
	void AddStatTagStack(URSItemInstance* ItemInstance, const TArray<FRarityStatRangeSet>& RarityStatRangeSets) const;

public:
	bool IsEquipableClassType(ECharacterClassType ClassType) const;
	
public:
	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	UPROPERTY(EditDefaultsOnly, meta=(Bitmask, BitmaskEnum="ECharacterClassType"))
	uint32 EquipableClassFlags = ((1 << (uint32)ECharacterClassType::Count) - 1);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const ULyraAbilitySet> BaseAbilitySet;
};
