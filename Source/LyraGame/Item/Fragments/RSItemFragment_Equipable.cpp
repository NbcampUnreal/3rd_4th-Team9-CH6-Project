#include "RSItemFragment_Equipable.h"

#include "Item/RSItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_Equipable)

FRarityStatSet::FRarityStatSet()
{
	RarityStats.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStats.Num(); i++)
	{
		RarityStats[i].Rarity = (EItemRarity)i;
	}
}

FRarityStatRangeSet::FRarityStatRangeSet()
{
	RarityStatRanges.SetNum((int32)EItemRarity::Count);
	for (int32 i = 0; i < RarityStatRanges.Num(); i++)
	{
		RarityStatRanges[i].Rarity = (EItemRarity)i;
	}
}

URSItemFragment_Equipable::URSItemFragment_Equipable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSItemFragment_Equipable::AddStatTagStack(URSItemInstance* ItemInstance, const TArray<FRarityStatSet>& RarityStatSets) const
{
	if (ItemInstance == nullptr)
		return;

	for (const FRarityStatSet& RarityStatSet : RarityStatSets)
	{
		const FGameplayTag& StatTag = RarityStatSet.StatTag;
		const FRarityStat& Stat = RarityStatSet.RarityStats[(int32)ItemInstance->GetItemRarity()];
		ItemInstance->AddStatTagStack(StatTag, Stat.Value);
	}
}

void URSItemFragment_Equipable::AddStatTagStack(URSItemInstance* ItemInstance, const TArray<FRarityStatRangeSet>& RarityStatRangeSets) const
{
	if (ItemInstance == nullptr)
		return;
	
	for (const FRarityStatRangeSet& RarityStatRangeSet : RarityStatRangeSets)
	{
		const FGameplayTag& StatTag = RarityStatRangeSet.StatTag;
		const FRarityStatRange& StatRange = RarityStatRangeSet.RarityStatRanges[(int32)ItemInstance->GetItemRarity()];
		const int32 StatValue = FMath::RandRange(StatRange.MinValue, StatRange.MaxValue);
		ItemInstance->AddStatTagStack(StatTag, StatValue);
	}
}

bool URSItemFragment_Equipable::IsEquipableClassType(ECharacterClassType ClassType) const
{
	return (EquipableClassFlags & (1 << (uint32)ClassType)) != 0;
}
