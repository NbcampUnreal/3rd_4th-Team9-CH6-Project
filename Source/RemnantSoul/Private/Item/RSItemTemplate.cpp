
#include "Item/RSItemTemplate.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemTemplate)

URSItemTemplate::URSItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
EDataValidationResult URSItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(TEXT("MaxStackCount must be >= 1")));
		Result = EDataValidationResult::Invalid;
	}

	if (SlotSize.X < 1 || SlotSize.Y < 1)
	{
		Context.AddError(FText::FromString(TEXT("SlotSize must be at least 1x1")));
		Result = EDataValidationResult::Invalid;
	}

	// 여기서는 최소 검증만 하고,
	// "무기/방어구는 MaxStackCount == 1이어야 한다" 같은 세부 규칙은
	// 나중에 Equipable Fragment나 Weapon Template에서 추가해도 된다.

	return Result;
}
#endif

URSItemInstance* URSItemTemplate::CreateItemInstance(AActor* Owner, int32 Count) const
{
	if (Count <= 0)
	{
		return nullptr;
	}

	// Owner를 Outer로 두면 GC/저장 시 다루기 쉬움
	UObject* Outer = Owner ? static_cast<UObject*>(Owner) : GetTransientPackage();

	URSItemInstance* NewInstance = NewObject<URSItemInstance>(Outer);
	if (!NewInstance)
	{
		return nullptr;
	}

	NewInstance->Initialize(const_cast<URSItemTemplate*>(this), Count, Owner);

	// Fragment에게도 생성 훅 제공
	for (URSItemFragment* Frag : Fragments)
	{
		if (Frag)
		{
			Frag->OnInstanceCreated(NewInstance);
		}
	}

	return NewInstance;
}

const URSItemFragment* URSItemTemplate::FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		return nullptr;
	}

	for (URSItemFragment* Frag : Fragments)
	{
		if (Frag && Frag->IsA(FragmentClass))
		{
			return Frag;
		}
	}
	return nullptr;
}

FPrimaryAssetId URSItemTemplate::GetPrimaryAssetId() const
{
	// AssetManager에서 "Item" 타입으로 관리하고 싶다는 가정. - RS프로젝트에 AssetManager를 넣기에는 힘들수도 있을 것 같다.
	// 필요하면 "RSItem", "RSItemTemplate" 등으로 바꿔도 된다.
	static const FPrimaryAssetType ItemType(TEXT("Item"));
	return FPrimaryAssetId(ItemType, GetFName());
}
