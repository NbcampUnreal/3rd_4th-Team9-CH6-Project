// Source/ChronoVeil/Game/Inventory/CVInventoryManagerComponent.cpp

#include "Game/Inventory/CVInventoryManagerComponent.h"
#include "Game/Inventory/CVInventoryItemDefinition.h"
#include "Game/Inventory/CVInventoryItemInstance.h"

UCVInventoryItemInstance* FCVInventoryList::AddEntry(TSubclassOf<UCVInventoryItemDefinition> ItemDef)
{
	UCVInventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FCVInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UCVInventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	// iterating fragments and call callback to OnInstanceCreated()
	for (const UCVInventoryItemFragment* Fragment : GetDefault<UCVInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	Result = NewEntry.Instance;
	return Result;
}

UCVInventoryManagerComponent::UCVInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}

UCVInventoryItemInstance* UCVInventoryManagerComponent::AddItemDefinition(TSubclassOf<UCVInventoryItemDefinition> ItemDef)
{
	UCVInventoryItemInstance* Result = nullptr;
	if (ItemDef)
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}
