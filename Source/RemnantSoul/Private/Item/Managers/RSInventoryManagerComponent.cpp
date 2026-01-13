
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

	// YKJ Annotation : 슬롯 배열 초기화. 최소 1칸은 보장한다.
	MaxSlots = FMath::Max(MaxSlots, 1);
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
	return IsValidIndex(Index) ? Slots[Index] : nullptr;
}

bool URSInventoryManagerComponent::AddItemByTemplate(const URSItemTemplate* ItemTemplate, int32 Count, int32& OutAddedCount)
{
	OutAddedCount = 0;

	if (!ItemTemplate || Count <= 0)
	{
		return false;
	}

	int32 Remaining = Count;

	// 1) 먼저 기존 스택에 합치기
	const int32 AddedFromStack = AddToExistingStacks(ItemTemplate, Remaining);
	Remaining -= AddedFromStack;
	OutAddedCount += AddedFromStack;

	// 2) 남은 수량은 빈 슬롯에 새 인스턴스를 생성해서 넣기
	if (Remaining > 0)
	{
		const int32 AddedFromNew = AddToEmptySlots(ItemTemplate, Remaining);
		Remaining -= AddedFromNew;
		OutAddedCount += AddedFromNew;
	}

	return OutAddedCount > 0;
}

bool URSInventoryManagerComponent::AbsorbItemInstanceCount(URSItemInstance* ItemInstance, int32& OutAddedCount)
{
	OutAddedCount = 0;

	if (!ItemInstance)
	{
		return false;
	}

	const URSItemTemplate* Template = ItemInstance->GetTemplate();
	if (!Template)
	{
		return false;
	}

	int32 Remaining = ItemInstance->GetCount();
	if (Remaining <= 0)
	{
		return false;
	}

	// 1) 기존 스택에 합치기
	const int32 AddedFromStack = AddToExistingStacks(Template, Remaining);
	Remaining -= AddedFromStack;
	OutAddedCount += AddedFromStack;

	// 2) 남은 수량은 Template 기반으로 새 인스턴스를 생성해서 채운다
	if (Remaining > 0)
	{
		int32 TmpAdded = 0;
		AddItemByTemplate(Template, Remaining, TmpAdded);
		OutAddedCount += TmpAdded;
	}

	return OutAddedCount > 0;
}


bool URSInventoryManagerComponent::AddItemInstance(URSItemInstance* ItemInstance, int32& OutAddedCount)
{
	// YKJ Annotation : 기존 호출부 호환용. 앞으로는 AbsorbItemInstanceCount를 사용한다.
	return AbsorbItemInstanceCount(ItemInstance, OutAddedCount);
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

URSItemInstance* URSInventoryManagerComponent::CreateItemInstance(const URSItemTemplate* Template, int32 Count, AActor* OwningActor)
{
	if (!Template || Count <= 0 || !OwningActor)
	{
		return nullptr;
	}

	URSItemInstance* NewInst = NewObject<URSItemInstance>(OwningActor);
	if (!NewInst)
	{
		return nullptr;
	}

	NewInst->InitializeFromTemplate(Template, Count, OwningActor); // 너 프로젝트의 실제 초기화 함수명에 맞춰
	return NewInst;
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

		// YKJ Annotation : 템플릿에 정의된 ItemTags에서 태그를 검색한다.
		if (Template->ItemTags.HasTag(Tag))
		{
			OutItems.Add(Instance);
		}
	}
}

int32 URSInventoryManagerComponent::AddToExistingStacks(const URSItemTemplate* Template, int32 Count)
{
	if (!Template || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;
	const int32 MaxStack = Template->GetMaxStackCount();

	// YKJ Annotation : 동일 템플릿 스택이 이미 존재하면, 우선 채워넣는다.
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

int32 URSInventoryManagerComponent::AddToEmptySlots(const URSItemTemplate* Template, int32 Count)
{
	if (!Template || Count <= 0)
	{
		return 0;
	}

	int32 Remaining = Count;
	const int32 MaxStack = Template->GetMaxStackCount();
	int32 TotalAdded = 0;

	// YKJ Annotation : 빈 슬롯에 새 인스턴스를 생성해서 꽂는다.
	for (int32 i = 0; i < Slots.Num() && Remaining > 0; ++i)
	{
		if (Slots[i])
		{
			continue;
		}

		const int32 ToCreateCount = FMath::Min(MaxStack, Remaining);

		URSItemInstance* NewInstance = Template->CreateItemInstance(GetOwner(), ToCreateCount);
		if (!NewInstance)
		{
			// YKJ Annotation : 생성 실패면 더 진행해도 의미가 없다.
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

	const int32 CurrentCount = Instance->GetCount();
	if (CurrentCount <= 0)
	{
		return 0;
	}

	const int32 ToRemove = FMath::Min(CurrentCount, CountToRemove);
	Instance->SetCount(CurrentCount - ToRemove);

	// YKJ Annotation : 0이 되면 Slots에서 제거한다.
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

	return ToRemove;
}
