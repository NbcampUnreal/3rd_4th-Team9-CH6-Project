#include "RSUtilitySlotWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "Character/LyraCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/RSItemData.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSUtilitySlotWidget)

URSUtilitySlotWidget::URSUtilitySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSUtilitySlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	FText SlotNumber = FText::AsNumber((int32)EWeaponSlotType::Count + (int32)WidgetUtilitySlotType + 1);
	Text_SlotNumber->SetText(SlotNumber);
}

void URSUtilitySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwningPlayerPawn());
	if (LyraCharacter == nullptr)
		return;
	
	EquipManager = LyraCharacter->GetComponentByClass<URSEquipManagerComponent>();
	EquipmentManager = LyraCharacter->GetComponentByClass<URSEquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return;

	const TArray<FRSEquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FRSEquipmentEntry& Entry = Entries[i];
		if (URSItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged((EEquipmentSlotType)i, ItemInstance, Entry.GetItemCount());
		}
	}
	EntryChangedDelegateHandle = EquipmentManager->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
	
	EEquipState CurrentEquipState = EquipManager->GetCurrentEquipState();
	OnEquipStateChanged(CurrentEquipState, CurrentEquipState);
	EquipStateChangedDelegateHandle = EquipManager->OnEquipStateChanged.AddUObject(this, &ThisClass::OnEquipStateChanged);
}

void URSUtilitySlotWidget::NativeDestruct()
{
	if (EquipmentManager)
	{
		EquipmentManager->OnEquipmentEntryChanged.Remove(EntryChangedDelegateHandle);
		EntryChangedDelegateHandle.Reset();
	}

	if (EquipManager)
	{
		EquipManager->OnEquipStateChanged.Remove(EquipStateChangedDelegateHandle);
		EquipStateChangedDelegateHandle.Reset();
	}
	
	Super::NativeDestruct();
}

void URSUtilitySlotWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount)
{
	EUtilitySlotType EntryUtilitySlotType = URSEquipManagerComponent::ConvertToUtilitySlotType(EquipmentSlotType);
	if (EntryUtilitySlotType != WidgetUtilitySlotType)
		return;
	
	if (ItemInstance)
	{
		const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

		Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
		Image_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (ItemCount > 1)
		{
			Text_Count->SetText(FText::AsNumber(ItemCount));
			Text_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Text_Count->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		Image_Icon->SetVisibility(ESlateVisibility::Hidden);
		Text_Count->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URSUtilitySlotWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	EEquipState SlotEquipState = URSEquipManagerComponent::ConvertToEquipState(WidgetUtilitySlotType);

	if (NewEquipState == SlotEquipState)
	{
		PlayAnimationForward(Animation_Highlight_In);
	}
	else if (PrevEquipState == SlotEquipState)
	{
		PlayAnimationReverse(Animation_Highlight_In);
	}
}
