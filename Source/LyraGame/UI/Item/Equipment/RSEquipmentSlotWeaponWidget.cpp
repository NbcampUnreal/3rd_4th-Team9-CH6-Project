#include "RSEquipmentSlotWeaponWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "RSDefine.h"
#include "Item/RSItemInstance.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "RSEquipmentEntryWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Data/RSUIData.h"
#include "Item/Fragments/RSItemFragment_Equipable_Weapon.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"
#include "UI/Item/RSItemDragDrop.h"
#include "UI/Item/Inventory/RSInventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentSlotWeaponWidget)

URSEquipmentSlotWeaponWidget::URSEquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void URSEquipmentSlotWeaponWidget::Init(EWeaponSlotType InWeaponSlotType, URSEquipmentManagerComponent* InEquipmentManager)
{
	check(InWeaponSlotType != EWeaponSlotType::Count && InEquipmentManager != nullptr);
	
	WeaponSlotType = InWeaponSlotType;
	EquipmentManager = InEquipmentManager;
}

void URSEquipmentSlotWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EntryWidgets.SetNum((int32)EWeaponHandType::Count);
	SlotRedImages = { Image_Red_LeftHand, Image_Red_RightHand, Image_Red_TwoHand };
	SlotGreenImages = { Image_Green_LeftHand, Image_Green_RightHand, Image_Green_TwoHand };
	SlotOverlays = { Overlay_LeftHandEntry, Overlay_RightHandEntry, Overlay_TwoHandEntry };
}

bool URSEquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;
	
	bAlreadyHovered = true;
	
	URSItemDragDrop* ItemDragDrop = Cast<URSItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	URSItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	bool bIsValid = false;
	UImage* TargetImage = Image_Red_TwoHand;
	
	if (const URSItemFragment_Equipable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<URSItemFragment_Equipable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = URSEquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (URSInventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				FIntPoint OutToItemSlotPos;
				bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
			}
		}
		else if (URSEquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
			}
		}

		const int32 WeaponHandIndex = (int32)FromWeaponFragment->WeaponHandType;
		TargetImage = bIsValid ? SlotGreenImages[WeaponHandIndex] : SlotRedImages[WeaponHandIndex];
	}

	if (TargetImage)
	{
		TargetImage->SetVisibility(ESlateVisibility::Visible);
	}
	
	return true;
}

bool URSEquipmentSlotWeaponWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	OnDragEnded();
	
	URSItemDragDrop* ItemDragDrop = Cast<URSItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	if (URSItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
	{
		FromEntryWidget->RefreshWidgetOpacity(true);
	}
	
	URSItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	URSItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<URSItemManagerComponent>();
	if (ItemManager == nullptr)
		return false;
	
	if (const URSItemFragment_Equipable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<URSItemFragment_Equipable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = URSEquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (URSInventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
		}
		else if (URSEquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
		}
	}
	return true;
}

void URSEquipmentSlotWeaponWidget::OnDragEnded()
{
	Super::OnDragEnded();

	for (UImage* SlotGreenImage : SlotGreenImages)
	{
		SlotGreenImage->SetVisibility(ESlateVisibility::Hidden);
	}

	for (UImage* SlotRedImage : SlotRedImages)
	{
		SlotRedImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URSEquipmentSlotWeaponWidget::OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, URSItemInstance* InItemInstance, int32 InItemCount)
{
	if (InWeaponHandType == EWeaponHandType::Count)
		return;

	const int32 WeaponHandIndex = (int32)InWeaponHandType;
	UOverlay* TargetOverlay = SlotOverlays[WeaponHandIndex];

	if (URSEquipmentEntryWidget* EntryWidget = EntryWidgets[WeaponHandIndex])
	{
		if (URSItemInstance* ItemInstance = EntryWidget->GetItemInstance())
		{
			if (ItemInstance == InItemInstance)
			{
				EntryWidget->RefreshItemCount(InItemCount);
				return;
			}
		}
		
		TargetOverlay->RemoveChild(EntryWidget);
		EntryWidgets[WeaponHandIndex] = nullptr;
	}

	int32 ActiveWidgetIndex = Switcher_WeaponHandEntry->GetActiveWidgetIndex();
	
	if (InItemInstance)
	{
		TSubclassOf<URSEquipmentEntryWidget> EntryWidgetClass = URSUIData::Get().EquipmentEntryWidgetClass;
		URSEquipmentEntryWidget* EntryWidget = CreateWidget<URSEquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[WeaponHandIndex] = EntryWidget;
		
		UOverlaySlot* OverlaySlot = TargetOverlay->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, URSEquipManagerComponent::ConvertToEquipmentSlotType(InWeaponHandType, WeaponSlotType), EquipmentManager);
		
		if (InWeaponHandType == EWeaponHandType::TwoHand)
			ActiveWidgetIndex = 1;
	}
	else
	{
		if (InWeaponHandType == EWeaponHandType::TwoHand)
			ActiveWidgetIndex = 0;
	}
	
	Switcher_WeaponHandEntry->SetActiveWidgetIndex(ActiveWidgetIndex);
}
