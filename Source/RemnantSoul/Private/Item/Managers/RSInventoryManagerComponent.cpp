#include "Item/Managers/RSInventoryManagerComponent.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventoryManagerComponent)

URSInventoryManagerComponent::URSInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 슬롯 배열 초기화
	if (MaxSlots < 1)
	{
		MaxSlots = 1;
	}

	Slots.SetNum(MaxSlots);
}

int32 URSInventoryManagerComponent::GetUsedSlotCount() const
{
	int32 Count = 0;
	for (const URSItemInstance* Instance : Slots)
	{
		if (Instance)
		{
			++Count;
		}
	}
	return Count;
}

bool URSInventoryManagerComponent::IsValidIndex(int32 Index) const
{
	return Slots.IsValidIndex(Index);
}

URSItemInstance* URSInventoryManagerComponent::GetItemAtIndex(int32 Index) const
{
	if (!IsValidIndex(Index))
	{
		return nullptr;
	}
	return Slots[Index];
}

bool URSInventoryManagerComponent::AddItemByTemplate(URSItemTemplate* ItemTemplate, int32 Count, int32& OutAddedCount)
{
	OutAddedCount = 0;

	if (!ItemTemplate || Count <= 0)
	{
		return false;
	}

	// 1) 먼저 기존 스택에 합치기
	int32 Remaining = Count;
	int32 AddedFromStack = AddToExistingStacks(ItemTemplate, Remaining);
	Remaining -= AddedFromStack;
	OutAddedCount += AddedFromStack;

	// 2) 남은 수량은 빈 슬롯에 새 인스턴스를 생성해서 넣기
	if (Remaining > 0)
	{
		int32 AddedFromNew = AddToEmptySlots(ItemTemplate, Remaining);
		Remaining -= AddedFromNew;
		OutAddedCount += AddedFromNew;
	}

	return OutAddedCount > 0;
}

bool URSInventoryManagerComponent::AddItemInstance(URSItemInstance* ItemInstance, int32& OutAddedCount)
{
	OutAddedCount = 0;

	if (!ItemInstance)
	{
		return false;
	}

	URSItemTemplate* Template = ItemInstance->GetTemplate();
	if (!Template)
	{
		return false;
	}

	// 인스턴스 안에 Count가 있다고 가정
	int32 Remaining = ItemInstance->GetCount();
	if (Remaining <= 0)
	{
		return false;
	}

	// 1) 기존 스택에 합치기
	int32 AddedFromStack = AddToExistingStacks(Template, Remaining);
	Remaining -= AddedFromStack;
	OutAddedCount += AddedFromStack;

	// 2) 남은 수량은 Template 기반으로 새 인스턴스를 생성해서 채운다.
	if (Remaining > 0)
	{
		int32 TmpAdded = 0;
		AddItemByTemplate(Template, Remaining, TmpAdded);
		OutAddedCount += TmpAdded;
		Remaining -= TmpAdded;
	}

	// 현재 구현에서는 넘어온 ItemInstance 자체는 슬롯에 직접 박지 않고,
	// Count만 내부 구조에 맞게 재분배하는 방식.
	// 필요하면 "외부 인스턴스를 그대로 슬롯에 넣는" Insert API를 별도로 추가 가능.

	return OutAddedCount > 0;
}

bool URSInventoryManagerComponent::RemoveItemByTemplate(URSItemTemplate* ItemTemplate, int32 CountToRemove, int32& OutRemovedCount)
{
	OutRemovedCount = 0;

	if (!ItemTemplate || CountToRemove <= 0)
	{
		return false;
	}

	OutRemovedCount = RemoveFromStacksByTemplate(ItemTemplate, CountToRemove);
	return OutRemovedCount > 0;
}

bool URSInventoryManagerComponent::RemoveItemInstance(URSItemInstance* ItemInstance, int32 CountToRemove, int32& OutRemovedCount)
{
	OutRemovedCount = 0;

	if (!ItemInstance || CountToRemove <= 0)
	{
		return false;
	}

	OutRemovedCount = RemoveFromInstance(ItemInstance, CountToRemove);
	return OutRemovedCount > 0;
}

URSItemInstance* URSInventoryManagerComponent::FindFirstItemByTemplate(URSItemTemplate* ItemTemplate) const
{
	if (!ItemTemplate)
	{
		return nullptr;
	}

	for (URSItemInstance* Instance : Slots)
	{
		if (Instance && Instance->GetTemplate() == ItemTemplate)
		{
			return Instance;
		}
	}

	return nullptr;
}

void URSInventoryManagerComponent::FindItemsWithTag(FGameplayTag Tag, TArray<URSItemInstance*>& OutItems) const
{
	OutItems.Reset();

	if (!Tag.IsValid())
	{
		return;
	}

	for (URSItemInstance* Instance : Slots)
	{
		if (!Instance)
		{
			continue;
		}

		const URSItemTemplate* Template = Instance->GetTemplate();
		if (!Template)
		{
			continue;
		}

		// 템플릿에 정의된 ItemTags에서 태그를 검색
		if (Template->ItemTags.HasTag(Tag))
		{
			OutItems.Add(Instance);
		}
	}
}

int32 URSInventoryManagerComponent::AddToExistingStacks(URSItemTemplate* Template, int32 Count)
{
	if (!Template || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;
	const int32 MaxStack = Template->GetMaxStackCount();

	// 동일 템플릿을 가진 스택에 우선 채워넣기
	for (URSItemInstance* Instance : Slots)
	{
		if (!Instance)
		{
			continue;
		}

		if (Instance->GetTemplate() != Template)
		{
			continue;
		}

		const int32 CurrentCount = Instance->GetCount();
		if (CurrentCount >= MaxStack)
		{
			continue;
		}

		const int32 FreeSpace = MaxStack - CurrentCount;
		const int32 ToAdd = FMath::Min(FreeSpace, Remaining);

		Instance->SetCount(CurrentCount + ToAdd);
		Remaining -= ToAdd;

		if (Remaining <= 0)
		{
			break;
		}
	}

	return Count - Remaining;
}

int32 URSInventoryManagerComponent::AddToEmptySlots(URSItemTemplate* Template, int32 Count)
{
	if (!Template || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;
	const int32 MaxStack = Template->GetMaxStackCount();
	int32 TotalAdded = 0;

	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		if (Slots[i])
		{
			continue;
		}

		const int32 ToCreateCount = FMath::Min(MaxStack, Remaining);

		// Template에서 새 인스턴스 생성
		URSItemInstance* NewInstance = Template->CreateItemInstance(GetOwner(), ToCreateCount);
		if (!NewInstance)
		{
			// 생성 실패 시 더 이상 진행하지 않음
			break;
		}

		Slots[i] = NewInstance;

		Remaining -= ToCreateCount;
		TotalAdded += ToCreateCount;
	}

	return TotalAdded;
}

int32 URSInventoryManagerComponent::RemoveFromStacksByTemplate(URSItemTemplate* Template, int32 CountToRemove)
{
	if (!Template || CountToRemove <= 0)
	{
		return 0;
	}

	int32 Remaining = CountToRemove;
	int32 TotalRemoved = 0;

	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		URSItemInstance* Instance = Slots[i];
		if (!Instance)
		{
			continue;
		}

		if (Instance->GetTemplate() != Template)
		{
			continue;
		}

		const int32 CurrentCount = Instance->GetCount();
		if (CurrentCount <= 0)
		{
			Slots[i] = nullptr;
			continue;
		}

		const int32 ToRemove = FMath::Min(CurrentCount, Remaining);
		Instance->SetCount(CurrentCount - ToRemove);

		Remaining -= ToRemove;
		TotalRemoved += ToRemove;

		if (Instance->GetCount() <= 0)
		{
			Slots[i] = nullptr;
		}
	}

	return TotalRemoved;
}

int32 URSInventoryManagerComponent::RemoveFromInstance(URSItemInstance* Instance, int32 CountToRemove)
{
	if (!Instance || CountToRemove <= 0)
	{
		return 0;
	}

	int32 TotalRemoved = 0;

	const int32 CurrentCount = Instance->GetCount();
	if (CurrentCount <= 0)
	{
		return 0;
	}

	const int32 ToRemove = FMath::Min(CurrentCount, CountToRemove);
	Instance->SetCount(CurrentCount - ToRemove);
	TotalRemoved += ToRemove;

	// 0이 되면 Slots에서 제거
	if (Instance->GetCount() <= 0)
	{
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (Slots[i] == Instance)
			{
				Slots[i] = nullptr;
				break;
			}
		}
	}

	return TotalRemoved;
}
