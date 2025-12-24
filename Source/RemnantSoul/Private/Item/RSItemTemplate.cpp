#include "Item/RSItemTemplate.h"

#include "Item/RSItemInstance.h"
#include "Item/Fragments/RSItemFragment.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemTemplate)

URSItemTemplate::URSItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
EDataValidationResult URSItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	// MaxStackCount 검증
	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(TEXT("MaxStackCount must be greater than or equal to 1.")));
		Result = EDataValidationResult::Invalid;
	}

	// 무기/방어구 타입인데 SlotTag가 비어있으면 경고
	if ((ItemType == ERSItemType::Weapon || ItemType == ERSItemType::Armor) && !SlotTag.IsValid())
	{
		Context.AddWarning(FText::FromString(TEXT("Weapon/Armor item should have a valid SlotTag.")));
		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	// Fragment 개별 검증이 필요하면 여기서 돌려도 됨.
	// for (URSItemFragment* Fragment : Fragments)
	// {
	//     if (Fragment)
	//     {
	//         // Fragment 쪽 IsDataValid를 호출하는 패턴으로 확장 가능
	//     }
	// }

	return Result;
}
#endif // WITH_EDITOR

URSItemInstance* URSItemTemplate::CreateItemInstance(AActor* OwningActor, int32 Count) const
{
	if (Count <= 0)
	{
		return nullptr;
	}

	// Owner가 없으면 Transient 패키지에 생성
	UObject* Outer = OwningActor ? Cast<UObject>(OwningActor) : GetTransientPackage();

	URSItemInstance* NewInstance = NewObject<URSItemInstance>(Outer, URSItemInstance::StaticClass());
	if (!NewInstance)
	{
		return nullptr;
	}

	NewInstance->Initialize(const_cast<URSItemTemplate*>(this), Count, OwningActor);

	// Fragment들에게 "인스턴스가 만들어졌다" 알림 (D1 스타일)
	for (URSItemFragment* Fragment : Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewInstance);
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

	for (URSItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}

	return nullptr;
}
