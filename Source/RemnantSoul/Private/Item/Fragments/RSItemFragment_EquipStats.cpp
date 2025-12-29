#include "Item/Fragments/RSItemFragment_EquipStats.h"

#include "Item/RSItemTemplate.h"
#include "Item/RSItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_EquipStats)

URSItemFragment_EquipStats::URSItemFragment_EquipStats(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSItemFragment_EquipStats::CollectModifiers(TArray<FRSItemEquipStatModifier>& OutModifiers) const
{
	for (const FRSItemEquipStatModifier& M : Modifiers)
	{
		if (M.IsValid())
		{
			OutModifiers.Add(M);
		}
	}
}

void URSItemFragment_EquipStats::GetAccumulatedForAttribute(
	FGameplayAttribute Attribute,
	float& OutAdditive,
	float& OutMultiplicative,
	bool& bHasOverride,
	float& OutOverrideValue
) const
{
	OutAdditive = 0.0f;
	OutMultiplicative = 1.0f;   // 곱연산의 기본값은 1임.,
	bHasOverride = false;
	OutOverrideValue = 0.0f;

	if (!Attribute.IsValid())
	{
		return;
	}

	for (const FRSItemEquipStatModifier& Modif : Modifiers)
	{
		if (!Modif.IsValid() || Modif.Attribute != Attribute)
		{
			continue;
		}

		switch (Modif.Operation)
		{
		case ERSItemStatModifierOp::Additive:
			OutAdditive += Modif.Magnitude;
			break;

		case ERSItemStatModifierOp::Multiplicative:
			// 예: 기존 1.0에서 1.1, 1.2가 들어오면 최종 1.32
			OutMultiplicative *= Modif.Magnitude;
			break;

		case ERSItemStatModifierOp::Override:
			bHasOverride = true;
			OutOverrideValue = Modif.Magnitude;
			break;

		default:
			break;
		}
	}
}

#if WITH_EDITOR
EDataValidationResult URSItemFragment_EquipStats::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (int32 Index = 0; Index < Modifiers.Num(); ++Index)
	{
		const FRSItemEquipStatModifier& Modif = Modifiers[Index];

		if (!Modif.Attribute.IsValid())
		{
			Context.AddError(
				FText::FromString(FString::Printf(TEXT("EquipStats Modifier[%d] has invalid Attribute"), Index))
			);
			Result = EDataValidationResult::Invalid;
		}

		if (Modif.Operation == ERSItemStatModifierOp::Multiplicative && Modif.Magnitude <= 0.f)
		{
			Context.AddWarning(
				FText::FromString(FString::Printf(TEXT("EquipStats Modifier[%d]: Multiplicative Magnitude <= 0 (maybe wrong?)"), Index))
			);
			if (Result == EDataValidationResult::NotValidated)
			{
				Result = EDataValidationResult::Valid;
			}
		}
	}

	return Result;
}
#endif
