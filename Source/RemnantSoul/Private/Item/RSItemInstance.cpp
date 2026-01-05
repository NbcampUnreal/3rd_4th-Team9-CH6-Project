#include "Item/RSItemInstance.h"

#include "Item/RSItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemInstance)

URSItemInstance::URSItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSItemInstance::Initialize(URSItemTemplate* InTemplate, int32 InCount, AActor* InOwningActor)
{
	ItemTemplate = InTemplate;
	OwningActor = InOwningActor;

	// 최소 0 이상으로 정리
	if (InCount < 0)
	{
		InCount = 0;
	}

	Count = InCount;

	// 나중에 Fragment에서 OnInstanceCreated 같은 훅을 쓰고 싶으면,
	// 여기서 템플릿에게 위임하면 된다.
	// 예:
	// if (ItemTemplate)
	// {
	//     ItemTemplate->OnInstanceCreated(this);
	// }
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