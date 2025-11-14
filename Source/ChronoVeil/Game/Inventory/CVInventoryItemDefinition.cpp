#include "Game/Inventory/CVInventoryItemDefinition.h"

UCVInventoryItemDefinition::UCVInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UCVInventoryItemFragment* UCVInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UCVInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UCVInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}
