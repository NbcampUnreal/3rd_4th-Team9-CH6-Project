#include "RSItemData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_Equipable_Armor.h"
#include "Item/Fragments/RSItemFragment_Equipable_Weapon.h"
#include "System/LyraAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemData)

const URSItemData& URSItemData::Get()
{
	return ULyraAssetManager::Get().GetItemData();
}

#if WITH_EDITORONLY_DATA
void URSItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B)
	{
		return A < B;
	});

	ItemTemplateClassToID.Empty();
	WeaponItemTemplateClasses.Empty();
	ArmorItemTemplateClasses.Empty();
	
	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);

		const URSItemTemplate* ItemTemplate = Pair.Value.GetDefaultObject();
		if (const URSItemFragment_Equipable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<URSItemFragment_Equipable_Weapon>())
		{
			if (WeaponFragment->WeaponType != EWeaponType::Unarmed)
			{
				WeaponItemTemplateClasses.Add(Pair.Value);
			}
		}
		else if (ItemTemplate->FindFragmentByClass<URSItemFragment_Equipable_Armor>())
		{
			ArmorItemTemplateClasses.Add(Pair.Value);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
EDataValidationResult URSItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<int32> ItemTemplateIDSet;
	TSet<TSubclassOf<URSItemTemplate>> ItemTemplateClassSet;

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check
		const int32 ItemTemplateID = Pair.Key;
		
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateIDSet.Add(ItemTemplateID);

		// Class Check
		const TSubclassOf<URSItemTemplate> ItemTemplateClass = Pair.Value;
		
		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateClassSet.Add(ItemTemplateClass);
	}
	return Result;
}
#endif // WITH_EDITOR

const URSItemTemplate& URSItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<URSItemTemplate>* ItemTemplateClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemTemplateClass, TEXT("Can't find ItemTemplateClass from ID [%d]"), ItemTemplateID);
	return *(ItemTemplateClass->GetDefaultObject());
}

int32 URSItemData::FindItemTemplateIDByClass(TSubclassOf<URSItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}

void URSItemData::GetAllItemTemplateClasses(TArray<TSubclassOf<URSItemTemplate>>& OutItemTemplateClasses) const
{
	OutItemTemplateClasses.Reset();
	OutItemTemplateClasses.Reserve(ItemTemplateIDToClass.Num());
	
	for (auto& Pair : ItemTemplateIDToClass)
	{
		OutItemTemplateClasses.Add(Pair.Value);
	}
}
