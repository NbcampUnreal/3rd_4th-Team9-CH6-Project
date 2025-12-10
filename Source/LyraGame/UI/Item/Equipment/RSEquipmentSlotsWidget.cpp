#include "RSEquipmentSlotsWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "RSEquipmentSlotSingleWidget.h"
#include "RSEquipmentSlotWeaponWidget.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentSlotsWidget)

URSEquipmentSlotsWidget::URSEquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSEquipmentSlotsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WeaponSlotWidgets  = { Widget_Weapon_Primary, Widget_Weapon_Secondary };
	ArmorSlotWidgets   = { Widget_Armor_Head, Widget_Armor_Chest, Widget_Armor_Legs, Widget_Armor_Hand, Widget_Armor_Foot };
	UtilitySlotWidgets = { Widget_Utility_Primary, Widget_Utility_Secondary, Widget_Utility_Tertiary, Widget_Utility_Quaternary };
}

void URSEquipmentSlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void URSEquipmentSlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
	
	Super::NativeDestruct();
}

void URSEquipmentSlotsWidget::ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message)
{
	if (Message.EquipmentManager == nullptr || Message.EquipManager == nullptr)
		return;

	EquipmentManager = Message.EquipmentManager;
	EquipManager = Message.EquipManager;
	
	for (int32 i = 0; i < WeaponSlotWidgets.Num(); i++)
	{
		WeaponSlotWidgets[i]->Init((EWeaponSlotType)i, EquipmentManager);
	}
	
	for (int32 i = 0; i < ArmorSlotWidgets.Num(); i++)
	{
		ArmorSlotWidgets[i]->Init((EArmorType)i, EquipmentManager);
	}

	for (int32 i = 0; i < UtilitySlotWidgets.Num(); i++)
	{
		UtilitySlotWidgets[i]->Init((EUtilitySlotType)i, EquipmentManager);
	}

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

void URSEquipmentSlotsWidget::DestructUI()
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

	for (URSEquipmentSlotWeaponWidget* SlotWeaponWidget : WeaponSlotWidgets)
	{
		if (SlotWeaponWidget)
		{
			for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
			{
				SlotWeaponWidget->OnEquipmentEntryChanged((EWeaponHandType)i, nullptr, 0);
			}
		}
	}

	for (URSEquipmentSlotSingleWidget* SlotArmorWidget : ArmorSlotWidgets)
	{
		if (SlotArmorWidget)
		{
			SlotArmorWidget->OnEquipmentEntryChanged(nullptr, 0);
		}
	}

	for (URSEquipmentSlotSingleWidget* SlotUtilityWidget : UtilitySlotWidgets)
	{
		if (SlotUtilityWidget)
		{
			SlotUtilityWidget->OnEquipmentEntryChanged(nullptr, 0);
		}
	}
}

void URSEquipmentSlotsWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (URSEquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
	{
		const int32 WeaponSlotIndex = (int32)URSEquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
		if (WeaponSlotWidgets.IsValidIndex(WeaponSlotIndex))
		{
			if (URSEquipmentSlotWeaponWidget* WeaponSlotWidget = WeaponSlotWidgets[WeaponSlotIndex])
			{
				EWeaponHandType WeaponHandType = URSEquipManagerComponent::ConvertToWeaponHandType(EquipmentSlotType);
				WeaponSlotWidget->OnEquipmentEntryChanged(WeaponHandType, ItemInstance, ItemCount);
			}
		}
	}
	else if (URSEquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
	{
		const int32 ArmorSlotIndex = (int32)URSEquipManagerComponent::ConvertToArmorType(EquipmentSlotType);
		if (ArmorSlotWidgets.IsValidIndex(ArmorSlotIndex))
		{
			if (URSEquipmentSlotSingleWidget* ArmorSlotWidget = ArmorSlotWidgets[ArmorSlotIndex])
			{
				ArmorSlotWidget->OnEquipmentEntryChanged(ItemInstance, ItemCount);
			}
		}
	}
	else if (URSEquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
	{
		const int32 UtilitySlotIndex = (int32)URSEquipManagerComponent::ConvertToUtilitySlotType(EquipmentSlotType);
		if (UtilitySlotWidgets.IsValidIndex(UtilitySlotIndex))
		{
			if (URSEquipmentSlotSingleWidget* UtilitySlotWidget = UtilitySlotWidgets[UtilitySlotIndex])
			{
				UtilitySlotWidget->OnEquipmentEntryChanged(ItemInstance, ItemCount);
			}
		}
	}
}

void URSEquipmentSlotsWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	switch (NewEquipState)
	{
	case EEquipState::Weapon_Primary:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(1);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(0);
		break;
	case EEquipState::Weapon_Secondary:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(0);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(1);
		break;
	default:
		Switcher_Weapon_Primary->SetActiveWidgetIndex(0);
		Switcher_Weapon_Secondary->SetActiveWidgetIndex(0);
	}
}
