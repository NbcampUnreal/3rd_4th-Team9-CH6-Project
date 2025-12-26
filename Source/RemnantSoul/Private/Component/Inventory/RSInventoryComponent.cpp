// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/RSInventoryComponent.h"
#include "ItemDataAsset//RSItemData.h"

// Sets default values for this component's properties
URSInventoryComponent::URSInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool URSInventoryComponent::HasItem(const URSItemData* ItemData) const
{
	return Items.ContainsByPredicate(
		[ItemData](const FInventoryItem& Item)
		{
			return Item.ItemData == ItemData;
		}
	);
}


FInventoryItem* URSInventoryComponent::FindItem(URSItemData* ItemData)
{
	return Items.FindByPredicate(
		[ItemData](const FInventoryItem& Item)
		{
			return Item.ItemData == ItemData;
		}
	);
}


bool URSInventoryComponent::AddItem(URSItemData* ItemData, int32 Count)
{
	
	if (!ItemData || Count <= 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Inventory] Invalid AddItem params"));
		return false;
	}

	if (FInventoryItem* Existing = FindItem(ItemData))
	{
		Existing->Count += Count;
		UE_LOG(LogTemp, Log,
			TEXT("[Inventory] Stack increased -> %d"),
			Existing->Count
		);
		
	}
	else
	{
		FInventoryItem NewItem;
		NewItem.ItemData = ItemData;
		NewItem.Count = Count;
		Items.Add(NewItem);
		UE_LOG(LogTemp, Log,
			TEXT("[Inventory] New item added"));
	}

	OnInventoryChanged.Broadcast();
	DebugPrintInventory();
	return true;
}

bool URSInventoryComponent::RemoveItem(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return false;
	}

	const int32 Index = Items.IndexOfByPredicate(
		[ItemData](const FInventoryItem& Item)
		{
			return Item.ItemData == ItemData;
		}
	);

	if (Index == INDEX_NONE)
	{
		return false;
	}

	FInventoryItem& Slot = Items[Index];

	if (Slot.Count < Count)
	{
		return false;
	}

	Slot.Count -= Count;

	if (Slot.Count <= 0)
	{
		Items.RemoveAtSwap(Index); // 순서 중요하면 RemoveAt
	}
	
	OnInventoryChanged.Broadcast();
	return true;
}

bool URSInventoryComponent::UseItem(int32 SlotIndex, AActor* User)
{
	OnInventoryChanged.Broadcast();
	return true;
}

void URSInventoryComponent::DebugPrintInventory() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Inventory Dump ==="));

	for (const FInventoryItem& Item : Items)
	{
		const FString Name = Item.ItemData ? Item.ItemData->DisplayName.ToString() : TEXT("NULL");
		UE_LOG(LogTemp, Log, TEXT("- %s x %d"), *Name, Item.Count);
	}
}


