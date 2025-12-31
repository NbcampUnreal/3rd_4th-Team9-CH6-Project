#include "Item/Fragments/RSItemFragment_CombatStyle.h"

#include "RSGameplayTags.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_CombatStyle)

URSCombatStyleData* URSItemFragment_CombatStyle::ResolveCombatStyle() const
{
	// 1. 명시 CombatStyle 최우선
	if (CombatStyle)
	{
		return CombatStyle;
	}

	// 2) (선택사항임.) StyleKeyTag -> ResolverTable로 찾기
	// 현재 테이블 Row 구조가 정의되지 않았으니 해당 프로젝트를 할때에는 미적용 예정.
	// 내가 원하면 다음 단계에서 "FRSCombatStyleRow { FGameplayTag Key; URSCombatStyleData* Style; }"
	// 같은 Row를 만들고 여기서 FindRow로 해결하면 될듯.
	//
	// if (StyleResolverTable && StyleKeyTag.IsValid())
	// {
	//     ...
	// }

	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult URSItemFragment_CombatStyle::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!CombatStyle && !StyleKeyTag.IsValid())
	{
		Context.AddWarning(FText::FromString(
			TEXT("CombatStyle Fragment: CombatStyle is null and StyleKeyTag is invalid. This item will not change style.")
		));

		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (StyleResolverTable && !StyleKeyTag.IsValid())
	{
		Context.AddWarning(FText::FromString(
			TEXT("CombatStyle Fragment: StyleResolverTable is set but StyleKeyTag is invalid.")
		));

		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}
#endif

