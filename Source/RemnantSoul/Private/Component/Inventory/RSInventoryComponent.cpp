// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/RSInventoryComponent.h"
#include "ItemDataAsset/RSItemData.h"

URSInventoryComponent::URSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Slots.SetNum(MaxSlots);

	for (FInventoryItem& Slot : Slots)
	{
		if (Slot.ItemData == nullptr || Slot.Count <= 0)
		{
			Slot.Clear();
			continue;
		}

		const int32 MaxStack = FMath::Max(1, Slot.ItemData->MaxStack);
		Slot.Count = FMath::Clamp(Slot.Count, 1, MaxStack);
	}
}

bool URSInventoryComponent::HasItem(const URSItemData* ItemData) const
{
	if (!ItemData)
	{
		return false;
	}

	return Slots.ContainsByPredicate(
		[ItemData](const FInventoryItem& Slot)
		{
			return Slot.ItemData == ItemData && Slot.Count > 0;
		}
	);
}

int32 URSInventoryComponent::FindFirstStackableSlot(const URSItemData* ItemData) const
{
	if (!ItemData)
	{
		return INDEX_NONE;
	}

	const int32 MaxStack = FMath::Max(1, ItemData->MaxStack);
	if (MaxStack <= 1)
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		const FInventoryItem& Slot = Slots[i];
		if (Slot.ItemData == ItemData && Slot.Count > 0 && Slot.Count < MaxStack)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int32 URSInventoryComponent::FindFirstEmptySlot() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty())
		{
			return i;
		}
	}
	return INDEX_NONE;
}

bool URSInventoryComponent::AddItem(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem: invalid params"));
		return false;
	}

	int32 Remaining = Count;
	const int32 MaxStack = FMath::Max(1, ItemData->MaxStack);

	// 1) 기존 스택 채우기
	while (Remaining > 0)
	{
		const int32 StackIndex = FindFirstStackableSlot(ItemData);
		if (StackIndex == INDEX_NONE)
		{
			break;
		}

		FInventoryItem& Slot = Slots[StackIndex];
		const int32 Space = MaxStack - Slot.Count;
		const int32 ToAdd = FMath::Min(Space, Remaining);
		Slot.Count += ToAdd;
		Remaining -= ToAdd;
	}

	// 2) 새 슬롯 생성
	while (Remaining > 0)
	{
		const int32 EmptyIndex = FindFirstEmptySlot();
		if (EmptyIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem: inventory full (Item=%s, Remaining=%d)"),
				*GetNameSafe(ItemData), Remaining);
			OnInventoryChanged.Broadcast();
			return false;
		}

		FInventoryItem& Slot = Slots[EmptyIndex];
		Slot.ItemData = ItemData;
		Slot.Count = FMath::Min(MaxStack, Remaining);
		Remaining -= Slot.Count;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool URSInventoryComponent::RemoveItem(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return false;
	}

	int32 Remaining = Count;

	for (FInventoryItem& Slot : Slots)
	{
		if (Remaining <= 0)
		{
			break;
		}

		if (Slot.ItemData != ItemData || Slot.Count <= 0)
		{
			continue;
		}

		const int32 ToRemove = FMath::Min(Slot.Count, Remaining);
		Slot.Count -= ToRemove;
		Remaining -= ToRemove;

		if (Slot.Count <= 0)
		{
			Slot.Clear();
		}
	}

	if (Remaining > 0)
	{
		return false;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool URSInventoryComponent::UseItem(int32 SlotIndex, AActor* User)
{
	if (!User)
	{
		return false;
	}

	if (!Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	FInventoryItem& Slot = Slots[SlotIndex];
	if (Slot.IsEmpty() || !Slot.ItemData)
	{
		return false;
	}

	// 1) 실제 사용 시도 (포션 효과 적용)
	const bool bUsed = Slot.ItemData->TryUse(User);
	if (!bUsed)
	{
		return false;
	}

	// 2) 사용 성공 시 소비
	if (Slot.ItemData->bConsumeOnUse)
	{
		Slot.Count -= 1;
		if (Slot.Count <= 0)
		{
			Slot.Clear();
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

void URSInventoryComponent::DebugPrintInventory() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Inventory Dump (Slots=%d) ==="), Slots.Num());

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		const FInventoryItem& Slot = Slots[i];
		if (Slot.IsEmpty())
		{
			continue;
		}

		const FString Name = Slot.ItemData ? Slot.ItemData->DisplayName.ToString() : TEXT("NULL");
		UE_LOG(LogTemp, Log, TEXT("[%d] %s x %d"), i, *Name, Slot.Count);
	}
}

