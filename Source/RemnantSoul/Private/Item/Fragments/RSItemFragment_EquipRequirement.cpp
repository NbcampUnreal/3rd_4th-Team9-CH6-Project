#include "Item/Fragments/RSItemFragment_EquipRequirement.h"

#include "Item/RSItemTemplate.h"
#include "Item/RSItemInstance.h"
#include "RSGameplayTags.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_EquipRequirement)

URSItemFragment_EquipRequirement::URSItemFragment_EquipRequirement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool URSItemFragment_EquipRequirement::CheckRequirements(
	const FGameplayTagContainer& OwnerTags,
	const FGameplayTagContainer& ItemTags,
	int32 CharacterLevel,
	FGameplayTagContainer& OutFailedReasonTags
) const
{
	OutFailedReasonTags.Reset();

	// 1. 레벨 체크
	if (MinCharacterLevel > 0 && CharacterLevel < MinCharacterLevel)
	{
		// 나중에 RSGameplayTags에 Equip 실패 태그 추가해서 여기에 AddTag 해도 됨
		return false;
	}

	if (MaxCharacterLevel > 0 && MaxCharacterLevel >= MinCharacterLevel && CharacterLevel > MaxCharacterLevel)
	{
		return false;
	}

	// 2. Owner Required Tags
	if (!RequiredOwnerTags.IsEmpty())
	{
		if (!OwnerTags.HasAll(RequiredOwnerTags))
		{
			OutFailedReasonTags.AppendTags(RequiredOwnerTags);
			return false;
		}
	}

	// 3. Owner Blocked Tags
	if (!BlockedOwnerTags.IsEmpty())
	{
		if (OwnerTags.HasAny(BlockedOwnerTags))
		{
			OutFailedReasonTags.AppendTags(BlockedOwnerTags);
			return false;
		}
	}

	// 4. Item Required Tags
	if (!RequiredItemTags.IsEmpty())
	{
		if (!ItemTags.HasAll(RequiredItemTags))
		{
			OutFailedReasonTags.AppendTags(RequiredItemTags);
			return false;
		}
	}

	// 5. Item Blocked Tags
	if (!BlockedItemTags.IsEmpty())
	{
		if (ItemTags.HasAny(BlockedItemTags))
		{
			OutFailedReasonTags.AppendTags(BlockedItemTags);
			return false;
		}
	}

	return true;
}

#if WITH_EDITOR
EDataValidationResult URSItemFragment_EquipRequirement::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (MaxCharacterLevel > 0 && MaxCharacterLevel < MinCharacterLevel)
	{
		Context.AddWarning(FText::FromString(TEXT("MaxCharacterLevel is less than MinCharacterLevel. MaxCharacterLevel will be ignored.")));

		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;
}
#endif
