#include "Item/RSItemInstance.h"

#include "Item/RSItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemInstance)

URSItemInstance::URSItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSItemInstance::InitializeFromTemplate(const URSItemTemplate* InTemplate, int32 InCount, AActor* InOwningActor)
{
	ItemTemplate = InTemplate;
	OwningActor = InOwningActor;

	if (InCount < 0) { InCount = 0; }
	Count = InCount;
}

void URSItemInstance::SetCount(int32 NewCount)
{
	if (NewCount < 0)
	{
		NewCount = 0;
	}

	Count = NewCount;
}

void URSItemInstance::AddCount(int32 Delta)
{
	// 단순 합
	const int32 NewCount = Count + Delta;
	SetCount(NewCount);
}

int32 URSItemInstance::GetMaxStackCount() const
{
	if (!ItemTemplate)
	{
		// 템플릿이 없으면 1 스택짜리로 취급
		return 1;
	}

	// 템플릿이 가진 MaxStackCount를 기반으로,
	// 최소값은 1로 보정
	const int32 TemplateMaxStack = ItemTemplate->GetMaxStackCount();
	return FMath::Max(TemplateMaxStack, 1);
}

bool URSItemInstance::IsStackFull() const
{
	return Count >= GetMaxStackCount();
}

bool URSItemInstance::IsValidInstance() const
{
	return (ItemTemplate != nullptr) && (Count > 0);
}

const URSCombatStyleData* URSItemInstance::GetItemCombatStyle() const
{
	return CombatStyle;
}