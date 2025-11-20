#include "Game/Inventory/CVInventoryItemInstance.h"
#include "Game/Inventory/CVInventoryItemDefinition.h"

UCVInventoryItemInstance::UCVInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UCVInventoryItemFragment* UCVInventoryItemInstance::FindFragmentByClass(TSubclassOf<UCVInventoryItemFragment> FragmentClass) const
{
	if (ItemDef == nullptr || !FragmentClass)
	{
		return nullptr;
	}

	const UCVInventoryItemDefinition* ItemCDO = GetDefault<UCVInventoryItemDefinition>(ItemDef);
	if (!ItemCDO)
	{
		return nullptr;
	}

	return ItemCDO->FindFragmentByClass(FragmentClass);
}

void UCVInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UCVInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

bool UCVInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag); // 또는 StatTags.HasTag(Tag);
}

int32 UCVInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}
