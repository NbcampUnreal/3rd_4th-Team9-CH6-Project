#include "Item/Fragments/RSItemFragment_AbilitySet.h"

#include "Item/RSItemInstance.h"
#include "RSGameplayTags.h"
// URSAbilitySet 선언부가 있는 헤더 (Lyra 스타일 AbilitySet)
// 예) #include "GAS/RSAbilitySet.h"
#include "GAS/AS/RSAbilitySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_AbilitySet)

URSItemFragment_AbilitySet::URSItemFragment_AbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSItemFragment_AbilitySet::GetAbilitySetsWithTag(FGameplayTag Tag, TArray<URSAbilitySet*>& OutSets) const
{
	OutSets.Reset();

	if (!Tag.IsValid())
	{
		for (const FRSItemAbilitySetEntry& Entry : AbilitySets)
		{
			if (Entry.AbilitySet)
			{
				OutSets.Add(Entry.AbilitySet);
			}
		}
		return;
	}

	for (const FRSItemAbilitySetEntry& Entry : AbilitySets)
	{
		if (!Entry.AbilitySet)
		{
			continue;
		}

		if (Entry.AbilityTags.HasTag(Tag))
		{
			OutSets.Add(Entry.AbilitySet);
		}
	}
}
