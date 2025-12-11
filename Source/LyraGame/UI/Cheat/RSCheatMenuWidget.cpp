#include "RSCheatMenuWidget.h"

#include "RSCheatEntryWidget.h"
#include "RSCheatListWidget.h"
#include "Components/Button.h"
#include "Data/RSCheatData.h"
#include "Data/RSItemData.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_Equipable.h"
#include "Item/Fragments/RSItemFragment_Equipable_Weapon.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCheatMenuWidget)

URSCheatMenuWidget::URSCheatMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSCheatMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CheatEntryWidgetClass = ULyraAssetManager::GetSubclassByName<URSCheatEntryWidget>("CheatEntryWidgetClass");
	Button_Exit->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitButtonClicked);
}

void URSCheatMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const URSItemData& ItemData = ULyraAssetManager::Get().GetItemData();

	TArray<TSubclassOf<URSItemTemplate>> ItemTemplateClasses;
	ItemData.GetAllItemTemplateClasses(ItemTemplateClasses);

	for (TSubclassOf<URSItemTemplate> ItemTemplateClass : ItemTemplateClasses)
	{
		const URSItemTemplate* ItemTemplate = ItemTemplateClass.GetDefaultObject();
		if (const URSItemFragment_Equipable* EquippableFragment = ItemTemplate->FindFragmentByClass<URSItemFragment_Equipable>())
		{
			if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
			{
				const URSItemFragment_Equipable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<URSItemFragment_Equipable_Weapon>();
				if (WeaponFragment->WeaponType == EWeaponType::Unarmed)
					continue;
				
				URSCheatEntryWidget* CheatEntryWidget = CreateWidget<URSCheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ERSCheatEntryType::PrimaryWeapon, ItemTemplateClass, nullptr);
				CheatList_PrimaryWeapon->AddEntry(CheatEntryWidget);

				CheatEntryWidget = CreateWidget<URSCheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ERSCheatEntryType::SecondaryWeapon, ItemTemplateClass, nullptr);
				CheatList_SecondaryWeapon->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Utility)
			{
				URSCheatEntryWidget* CheatEntryWidget = CreateWidget<URSCheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ERSCheatEntryType::Utility, ItemTemplateClass, nullptr);
				CheatList_Utility->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
			{
				URSCheatEntryWidget* CheatEntryWidget = CreateWidget<URSCheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(ERSCheatEntryType::Armor, ItemTemplateClass, nullptr);
				CheatList_Armor->AddEntry(CheatEntryWidget);
			}
		}
	}

	const URSCheatData& CheatData = ULyraAssetManager::Get().GetCheatData();
	
	for (const TSoftObjectPtr<UAnimMontage>& AnimMontagePath : CheatData.GetAnimMontagePaths())
	{
		URSCheatEntryWidget* CheatEntryWidget = CreateWidget<URSCheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
		CheatEntryWidget->InitializeUI(ERSCheatEntryType::Animation, nullptr, AnimMontagePath);
		CheatList_Animation->AddEntry(CheatEntryWidget);
	}
}

void URSCheatMenuWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
