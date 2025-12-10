#include "RSCheatEntryWidget.h"

#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Item/RSItemTemplate.h"
#include "Player/LyraCheatManager.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCheatEntryWidget)

URSCheatEntryWidget::URSCheatEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSCheatEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Entry->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void URSCheatEntryWidget::InitializeUI(ERSCheatEntryType InCheatEntryType, TSubclassOf<URSItemTemplate> InItemTemplateClass, TSoftObjectPtr<UAnimMontage> InAnimMontage)
{
	CheatEntryType = InCheatEntryType;
	
	switch (CheatEntryType)
	{
	case ERSCheatEntryType::PrimaryWeapon:
	case ERSCheatEntryType::SecondaryWeapon:
	case ERSCheatEntryType::Armor:
	case ERSCheatEntryType::Utility:
		ItemTemplateClass = InItemTemplateClass;
		if (ItemTemplateClass)
		{
			if (URSItemTemplate* ItemTemplate = Cast<URSItemTemplate>(ItemTemplateClass->GetDefaultObject()))
			{
				Text_Entry->SetText(ItemTemplate->DisplayName);
				Image_Entry->SetBrushFromTexture(ItemTemplate->IconTexture, true);
			}
		}
		break;
	case ERSCheatEntryType::Animation:
		AnimMontage = InAnimMontage;
		if (AnimMontage.IsNull() == false)
		{
			Text_Entry->SetText(FText::FromString(AnimMontage.GetAssetName()));
			SizeBox_Entry->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	}
}

void URSCheatEntryWidget::OnButtonClicked()
{
	ALyraPlayerController* LyraPlayerController = GetOwningPlayer<ALyraPlayerController>();
	if (LyraPlayerController == nullptr)
		return;

	switch (CheatEntryType)
	{
	case ERSCheatEntryType::PrimaryWeapon:
		// TEMP Rookiss
		//LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Primary, ItemTemplateClass);
		break;
	case ERSCheatEntryType::SecondaryWeapon:
		// TEMP Rookiss
		//LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Secondary, ItemTemplateClass);
		break;
	case ERSCheatEntryType::Armor:
		// TEMP Rookiss
		//LyraPlayerController->Server_EquipArmor(ItemTemplateClass);
		break;
	case ERSCheatEntryType::Utility:
		// TEMP Rookiss
		//LyraPlayerController->Server_EquipUtility(EUtilitySlotType::Primary, ItemTemplateClass);
		break;
	case ERSCheatEntryType::Animation:
		if (ULyraCheatManager* LyraCheatManager = Cast<ULyraCheatManager>(LyraPlayerController->CheatManager))
		{
			LyraCheatManager->SelectedMontage = AnimMontage;
		}
		break;
	}
}
