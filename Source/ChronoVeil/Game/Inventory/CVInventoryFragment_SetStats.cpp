#include "Game/Inventory/CVInventoryFragment_SetStats.h"
#include "Game/Inventory/CVInventoryItemInstance.h"

void UCVInventoryFragment_SetStats::OnInstanceCreated(UCVInventoryItemInstance* Instance) const
{
	if (!Instance)
	{
		return;
	}

	for (const auto& InitialItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(InitialItemStat.Key, InitialItemStat.Value);
	}
}
