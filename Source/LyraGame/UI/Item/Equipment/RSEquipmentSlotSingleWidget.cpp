#include "RSEquipmentSlotSingleWidget.h"

#include "RSEquipmentSlotsWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Item/RSItemInstance.h"
#include "Item/Fragments/RSItemFragment_Equipable_Armor.h"
#include "Item/Fragments/RSItemFragment_Equipable_Utility.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/RSItemDragDrop.h"
#include "UI/Item/Equipment/RSEquipmentEntryWidget.h"
#include "UI/Item/Inventory/RSInventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentSlotSingleWidget)

URSEquipmentSlotSingleWidget::URSEquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSEquipmentSlotSingleWidget::Init(EArmorType InArmorType, URSEquipmentManagerComponent* InEquipmentManager)
{
	check(InArmorType != EArmorType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Armor;
	ArmorType = InArmorType;
	EquipmentManager = InEquipmentManager;
}

void URSEquipmentSlotSingleWidget::Init(EUtilitySlotType InUtilitySlotType, URSEquipmentManagerComponent* InEquipmentManager)
{
	check(InUtilitySlotType != EUtilitySlotType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Utility;
	UtilitySlotType = InUtilitySlotType;
	EquipmentManager = InEquipmentManager;
}

void URSEquipmentSlotSingleWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image_BaseIcon)
	{
		Image_BaseIcon->SetBrushFromTexture(BaseIconTexture, true);
	}
}

bool URSEquipmentSlotSingleWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
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
	
	if (bIsValid)
	{
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
	}
	return true;
}

bool URSEquipmentSlotSingleWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const URSItemFragment* FromItemFragment = nullptr;
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	FromItemFragment = FromItemInstance->FindFragmentByClass<URSItemFragment_Equipable_Armor>();	break;
	case EEquipmentSingleSlotType::Utility:	FromItemFragment = FromItemInstance->FindFragmentByClass<URSItemFragment_Equipable_Utility>();	break;
	}
	if (FromItemFragment == nullptr)
		return false;
	
	if (URSInventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
	}
	else if (URSEquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
	}
	
	return true;
}

void URSEquipmentSlotSingleWidget::OnDragEnded()
{
	Super::OnDragEnded();

	Image_Red->SetVisibility(ESlateVisibility::Hidden);
	Image_Green->SetVisibility(ESlateVisibility::Hidden);
}

void URSEquipmentSlotSingleWidget::OnEquipmentEntryChanged(URSItemInstance* InItemInstance, int32 InItemCount)
{
	if (EntryWidget)
	{
		Overlay_Entry->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
	
	if (InItemInstance)
	{
		TSubclassOf<URSEquipmentEntryWidget> EntryWidgetClass = URSUIData::Get().EquipmentEntryWidgetClass;
		EntryWidget = CreateWidget<URSEquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		
		UOverlaySlot* OverlaySlot = Overlay_Entry->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, GetEquipmentSlotType(), EquipmentManager);

		Image_BaseIcon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_BaseIcon->SetRenderOpacity(1.f);
	}
}

EEquipmentSlotType URSEquipmentSlotSingleWidget::GetEquipmentSlotType() const
{
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	return URSEquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	case EEquipmentSingleSlotType::Utility:	return URSEquipManagerComponent::ConvertToEquipmentSlotType(UtilitySlotType);
	}

	return EEquipmentSlotType::Count;
}
