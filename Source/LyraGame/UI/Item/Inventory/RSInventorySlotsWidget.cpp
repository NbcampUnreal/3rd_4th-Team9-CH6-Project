#include "RSInventorySlotsWidget.h"

#include "AbilitySystemComponent.h"
#include "RSInventoryEntryWidget.h"
#include "RSInventorySlotWidget.h"
#include "RSInventoryValidWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Data/RSItemData.h"
#include "Data/RSUIData.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"
#include "UI/Item/RSItemDragDrop.h"
#include "UI/Item/Equipment/RSEquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventorySlotsWidget)

URSInventorySlotsWidget::URSInventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TitleText = FText::FromString(TEXT("Title"));
}

void URSInventorySlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void URSInventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void URSInventorySlotsWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
	
	Super::NativeDestruct();
}

bool URSInventorySlotsWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	
	URSItemDragDrop* DragDrop = Cast<URSItemDragDrop>(InOperation);
	if (DragDrop == nullptr)
		return false;

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	
	FVector2D MouseWidgetPos = GetSlotContainerGeometry().AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / UnitInventorySlotSize.X, ToWidgetPos.Y / UnitInventorySlotSize.Y);

	if (PrevDragOverSlotPos == ToItemSlotPos)
		return true;
	
	PrevDragOverSlotPos = ToItemSlotPos;
	
	URSItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	URSItemInstance* FromItemInstance = FromEntryWidget->GetItemInstance();
	if (FromItemInstance == nullptr)
		return false;
	
	const URSItemTemplate& FromItemTemplate = URSItemData::Get().FindItemTemplateByID(FromItemInstance->GetItemTemplateID());
	const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;

	int32 MovableCount = 0;
	if (URSInventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		MovableCount = InventoryManager->CanMoveOrMergeItem(FromInventoryManager, DragDrop->FromItemSlotPos, ToItemSlotPos);
	}
	else if (URSEquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		MovableCount = InventoryManager->CanMoveOrMergeItem(FromEquipmentManager, DragDrop->FromEquipmentSlotType, ToItemSlotPos);
	}
	
	ResetValidSlots();

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	const FIntPoint StartSlotPos = FIntPoint(FMath::Max(0, ToItemSlotPos.X), FMath::Max(0, ToItemSlotPos.Y));
	const FIntPoint EndSlotPos   = FIntPoint(FMath::Min(ToItemSlotPos.X + FromItemSlotCount.X, InventorySlotCount.X),
											 FMath::Min(ToItemSlotPos.Y + FromItemSlotCount.Y, InventorySlotCount.Y));

	ESlotState SlotState = (MovableCount > 0) ? ESlotState::Valid : ESlotState::Invalid;
	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (URSInventoryValidWidget* ValidWidget = ValidWidgets[Index])
			{
				ValidWidget->ChangeSlotState(SlotState);
			}
		}
	}
	return true;
}

void URSInventorySlotsWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	FinishDrag();
}

bool URSInventorySlotsWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	FinishDrag();

	FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
	
	URSItemDragDrop* DragDrop = Cast<URSItemDragDrop>(InOperation);
	check(DragDrop);

	URSItemEntryWidget* FromEntryWidget = DragDrop->FromEntryWidget;
	FromEntryWidget->RefreshWidgetOpacity(true);
	
	FVector2D MouseWidgetPos = GetSlotContainerGeometry().AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ToWidgetPos = MouseWidgetPos - DragDrop->DeltaWidgetPos;
	FIntPoint ToItemSlotPos = FIntPoint(ToWidgetPos.X / UnitInventorySlotSize.X, ToWidgetPos.Y / UnitInventorySlotSize.Y);

	DragDrop->ToInventoryManager = InventoryManager;
    DragDrop->ToItemSlotPos = ToItemSlotPos;

	URSItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<URSItemManagerComponent>();
	check(ItemManager);
	
	if (URSInventoryManagerComponent* FromInventoryManager = DragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToInventory(FromInventoryManager, DragDrop->FromItemSlotPos, InventoryManager, ToItemSlotPos);
	}
	else if (URSEquipmentManagerComponent* FromEquipmentManager = DragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToInventory(FromEquipmentManager, DragDrop->FromEquipmentSlotType, InventoryManager, ToItemSlotPos);
	}
	return true;
}

void URSInventorySlotsWidget::ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message)
{
	if (Message.InventoryManager == nullptr)
		return;
	
	InventoryManager = Message.InventoryManager;
	
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	const int32 InventorySlotNum = InventorySlotCount.X * InventorySlotCount.Y;
	SlotWidgets.SetNum(InventorySlotNum);
	ValidWidgets.SetNum(InventorySlotNum);
	EntryWidgets.SetNum(InventorySlotNum);
	
	for (int32 y = 0; y < InventorySlotCount.Y; y++)
	{
		for (int32 x = 0; x < InventorySlotCount.X; x++)
		{
			TSubclassOf<URSInventorySlotWidget> SlotWidgetClass = URSUIData::Get().InventorySlotWidgetClass;
			URSInventorySlotWidget* SlotWidget = CreateWidget<URSInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			SlotWidgets[y * InventorySlotCount.X + x] = SlotWidget;
			GridPanel_Slots->AddChildToUniformGrid(SlotWidget, y, x);

			TSubclassOf<URSInventoryValidWidget> ValidWidgetClass = URSUIData::Get().InventoryValidWidgetClass;
			URSInventoryValidWidget* ValidWidget = CreateWidget<URSInventoryValidWidget>(GetOwningPlayer(), ValidWidgetClass);
			ValidWidgets[y * InventorySlotCount.X + x] = ValidWidget;
			GridPanel_ValidSlots->AddChildToUniformGrid(ValidWidget, y, x);
		}
	}
	
	const TArray<FRSInventoryEntry>& Entries = InventoryManager->GetAllEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FRSInventoryEntry& Entry = Entries[i];
		if (URSItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
			OnInventoryEntryChanged(ItemSlotPos, ItemInstance, Entry.GetItemCount());
		}
	}
	EntryChangedDelegateHandle = InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void URSInventorySlotsWidget::DestructUI()
{
	InventoryManager->OnInventoryEntryChanged.Remove(EntryChangedDelegateHandle);
	EntryChangedDelegateHandle.Reset();

	CanvasPanel_Entries->ClearChildren();
	EntryWidgets.Reset();
	
	GridPanel_Slots->ClearChildren();
	SlotWidgets.Reset();
}

void URSInventorySlotsWidget::ResetValidSlots()
{
	for (URSInventoryValidWidget* ValidWidget : ValidWidgets)
	{
		if (ValidWidget)
		{
			ValidWidget->ChangeSlotState(ESlotState::Default);
		}
	}
}

void URSInventorySlotsWidget::FinishDrag()
{
	ResetValidSlots();
	PrevDragOverSlotPos = FIntPoint(-1, -1);
}

void URSInventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, URSItemInstance* InItemInstance, int32 InItemCount)
{
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;

	if (URSInventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		URSItemInstance* ItemInstance = EntryWidget->GetItemInstance();
		if (ItemInstance && ItemInstance == InItemInstance)
		{
			EntryWidget->RefreshItemCount(InItemCount);
			return;
		}
		
		CanvasPanel_Entries->RemoveChild(EntryWidget);
		EntryWidgets[SlotIndex] = nullptr;
	}
	
	if (InItemInstance)
	{
		FIntPoint UnitInventorySlotSize = URSUIData::Get().UnitInventorySlotSize;
		
		TSubclassOf<URSInventoryEntryWidget> EntryWidgetClass = URSUIData::Get().InventoryEntryWidgetClass;
		URSInventoryEntryWidget* EntryWidget = CreateWidget<URSInventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;
		
		EntryWidget->Init(this, InItemInstance, InItemCount);
		
		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitInventorySlotSize.X, InItemSlotPos.Y * UnitInventorySlotSize.Y));
	}
}

const FGeometry& URSInventorySlotsWidget::GetSlotContainerGeometry() const
{
	return Overlay_Slots->GetCachedGeometry();
}
