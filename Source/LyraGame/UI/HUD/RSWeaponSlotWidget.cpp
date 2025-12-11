#include "RSWeaponSlotWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "Animation/UMGSequencePlayer.h"
#include "Character/LyraCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/RSItemData.h"
#include "Item/RSItemTemplate.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSWeaponSlotWidget)

URSWeaponSlotWidget::URSWeaponSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSWeaponSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	FText SlotNumber = FText::AsNumber((int32)WidgetWeaponSlotType + 1);
	if (Text_SlotNumber)
	{
		Text_SlotNumber->SetText(SlotNumber);
	}
}

void URSWeaponSlotWidget::NativeConstruct()
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

void URSWeaponSlotWidget::NativeDestruct()
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

void URSWeaponSlotWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount)
{
	EWeaponSlotType EntryWeaponSlotType = URSEquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
	if (EntryWeaponSlotType != WidgetWeaponSlotType)
		return;
	
	EWeaponHandType EntryWeaponHandType = URSEquipManagerComponent::ConvertToWeaponHandType(EquipmentSlotType);
	if (EntryWeaponHandType == EWeaponHandType::LeftHand)
	{
		if (ItemInstance)
		{
			const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
			Image_TwoSlot_Left->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Left->SetVisibility(ESlateVisibility::HitTestInvisible);

			if (Switcher_Slots->GetActiveWidgetIndex() != 1)
			{
				PlayAnimationForward(Animation_ShowCrossLine);
				Switcher_Slots->SetActiveWidgetIndex(1);
			}
		}
		else
		{
			Image_TwoSlot_Left->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (EntryWeaponHandType == EWeaponHandType::RightHand)
	{
		if (ItemInstance)
		{
			const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
			Image_TwoSlot_Right->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_TwoSlot_Right->SetVisibility(ESlateVisibility::HitTestInvisible);

			if (Switcher_Slots->GetActiveWidgetIndex() != 1)
			{
				PlayAnimationForward(Animation_ShowCrossLine);
				Switcher_Slots->SetActiveWidgetIndex(1);
			}
		}
		else
		{
			Image_TwoSlot_Right->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (EntryWeaponHandType == EWeaponHandType::TwoHand)
	{
		if (ItemInstance)
		{
			const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
			Image_OneSlot->SetBrushFromTexture(ItemTemplate.IconTexture, true);
			Image_OneSlot->SetVisibility(ESlateVisibility::HitTestInvisible);

			if (Switcher_Slots->GetActiveWidgetIndex() != 0)
			{
				PlayAnimationReverse(Animation_ShowCrossLine);
				Switcher_Slots->SetActiveWidgetIndex(0);
			}
		}
		else
		{
			Image_OneSlot->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URSWeaponSlotWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	EEquipState SlotEquipState = URSEquipManagerComponent::ConvertToEquipState(WidgetWeaponSlotType);
	
	if (NewEquipState == SlotEquipState)
	{
		PlayAnimationForward(Animation_ExpandSlot);
	}
	else if (PrevEquipState == SlotEquipState)
	{
		PlayAnimationReverse(Animation_ExpandSlot);
	}
}
