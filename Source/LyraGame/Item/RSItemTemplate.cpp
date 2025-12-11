#include "RSItemTemplate.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Fragments/RSItemFragment_Equipable.h"
#include "Fragments/RSItemFragment_Equipable_Armor.h"
#include "Fragments/RSItemFragment_Equipable_Utility.h"
#include "Fragments/RSItemFragment_Equipable_Weapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemTemplate)

URSItemTemplate::URSItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#if WITH_EDITOR
EDataValidationResult URSItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (SlotCount.X < 1 || SlotCount.Y < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("SlotCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}

	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("MaxStackCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}

	const URSItemFragment_Equipable* FoundEquipable = nullptr;
	for (URSItemFragment* Fragment : Fragments)
	{
		if (URSItemFragment_Equipable* CurrentEquippable = Cast<URSItemFragment_Equipable>(Fragment))
		{
			if (FoundEquipable)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Equippable Fragment"))));
				return EDataValidationResult::Invalid;
			}

			FoundEquipable = CurrentEquippable;
		}
	}

	if (FoundEquipable)
	{
		if (FoundEquipable->EquipmentType == EEquipmentType::Count)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Equipment Type is Invalid : [EEquipmentType::Count]"))));
			return EDataValidationResult::Invalid;
		}

		if (FoundEquipable->EquipmentType == EEquipmentType::Armor)
		{
			const URSItemFragment_Equipable_Armor* ArmorFragment = Cast<URSItemFragment_Equipable_Armor>(FoundEquipable);

			if (ArmorFragment->ArmorType == EArmorType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor Type is Invalid : [EArmorType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			const URSItemFragment_Equipable_Weapon* WeaponFragment = Cast<URSItemFragment_Equipable_Weapon>(FoundEquipable);

			if (WeaponFragment->WeaponType == EWeaponType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Type is Invalid : [EWeaponType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}

			if (WeaponFragment->WeaponHandType == EWeaponHandType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Weapon Hand Type is Invalid : [EWeaponHandType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
		else if (FoundEquipable->EquipmentType == EEquipmentType::Utility)
		{
			const URSItemFragment_Equipable_Utility* UtilityFragment = Cast<URSItemFragment_Equipable_Utility>(FoundEquipable);

			if (UtilityFragment->UtilityType == EUtilityType::Count)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Utility Type is Invalid : [EUtilityType::Count]"))));
				Result = EDataValidationResult::Invalid;
			}
		}

		if (FoundEquipable->EquipmentType == EEquipmentType::Armor || FoundEquipable->EquipmentType == EEquipmentType::Weapon)
		{
			if (MaxStackCount != 1)
			{
				Context.AddError(FText::FromString(FString::Printf(TEXT("Armor or Weapon Type must have MaxStackCount of 1: [MaxStackCount != 1]"))));
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}
#endif // WITH_EDITOR

const URSItemFragment* URSItemTemplate::FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (URSItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
