// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerController/RSPlayerController.h"
#include "Blueprint/UserWidget.h"

//#include "Input/RSEnhancedInputComponent.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "IngameUI/inventory/RSInventoryWidget.h"
#include "GameFramework/Pawn.h"
#include "IngameUI/inventory//RSQuickSlotWidget.h"
#include "ItemDataAsset/RSItemData.h"

// ============================
// [MENU 추가] : ESC 키 바인딩용
// ============================
#include "InputCoreTypes.h"
// ============================

static const float PickupRange = 250.0f;

ARSPlayerController::ARSPlayerController()
{
	bShowMouseCursor = false;

	// [MENU 추가] 기본값(선언부에서도 false지만 명시)
	bMenuOpen = false;
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

// ============================
// [MENU 추가] : ESC를 C++에서 강제로 바인딩
// ============================
void ARSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	// ESC -> ToggleMenu
	FInputKeyBinding& KeyBind = InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ThisClass::ToggleMenu);
	KeyBind.bConsumeInput = true;
	KeyBind.bExecuteWhenPaused = true; // 메뉴 열고 Pause 상태에서도 ESC로 닫히게
}
// ============================

void ARSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InventoryComp = InPawn ? InPawn->FindComponentByClass<URSInventoryComponent>() : nullptr;
	if (InventoryComp)
	{
		InventoryComp->OnInventoryChanged.AddUObject(this, &ThisClass::HandleInventoryChanged);
	}
	EnsureQuickSlotWidgetCreated();
	HandleInventoryChanged();
}

void ARSPlayerController::UseItemFromSlot(int32 SlotIndex)
{
	APawn* Pawnd = GetPawn();
	if (!Pawnd)
	{
		return;
	}

	InventoryComp->UseItem(SlotIndex, Pawnd);
}

void ARSPlayerController::EnsureInventoryWidgetCreated()
{
	if (InventoryWidget) return;
	if (!InventoryWidgetClass) return;

	InventoryWidget = CreateWidget<URSInventoryWidget>(this, InventoryWidgetClass);
	if (!InventoryWidget) return;

	InventoryWidget->AddToViewport(10);
	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	if (InventoryComp)
	{
		InventoryWidget->Init(InventoryComp);
		UE_LOG(LogTemp, Warning, TEXT("[PC] Binding InventoryWidget->OnUseRequested"));

		InventoryWidget->OnUseRequested.AddUObject(this, &ThisClass::UseItemFromSlot);
		UE_LOG(LogTemp, Warning, TEXT("[PC] Bound OK. InventoryWidget=%s"), *GetNameSafe(InventoryWidget));
		bInventoryWidgetInited = true;
	}
}

void ARSPlayerController::ApplyInventoryInputMode(bool bOpen)
{
	if (bOpen)
	{
		bShowMouseCursor = true;

		FInputModeGameAndUI Mode;
		if (InventoryWidget)
		{
			Mode.SetWidgetToFocus(InventoryWidget->TakeWidget());
		}
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		SetInputMode(Mode);
	}
	else
	{
		bShowMouseCursor = false;

		FInputModeGameOnly Mode;
		SetInputMode(Mode);
	}
}

void ARSPlayerController::OpenInventoryUI()
{
	EnsureInventoryWidgetCreated();
	if (!InventoryWidget) return;

	bInventoryOpen = true;
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	ApplyInventoryInputMode(true);
}

void ARSPlayerController::CloseInventoryUI()
{
	if (!InventoryWidget)
	{
		bInventoryOpen = false;
		ApplyInventoryInputMode(false);
		return;
	}

	bInventoryOpen = false;
	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	ApplyInventoryInputMode(false);
}

void ARSPlayerController::ToggleInventoryUI()
{
	UE_LOG(LogTemp, Warning, TEXT("[Inv] ToggleInventoryUI called"));

	if (bInventoryOpen)
	{
		CloseInventoryUI();
		return;
	}

	OpenInventoryUI();
}

void ARSPlayerController::OnPlayerDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] Player Death UI"));

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ARSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARSPlayerController::ShowGameOverUI()
{
	// 이미 떠있으면 무시
	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		return;
	}

	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport(100); // 다른 UI보다 위에 뜨게 

			// 마우스 커서 보이게 설정 및 입력 모드 변경
			bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}

// ============================
// [MENU 추가] : 메뉴 입력 모드 처리 (GameAndUI + 이동/시점 입력 차단)
// ============================
void ARSPlayerController::ApplyMenuInputMode(bool bOpen)
{
	if (bOpen)
	{
		bShowMouseCursor = true;

		// 게임 입력(이동/시점) 차단
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);

		if (MenuWidgetInstance)
		{
			// 포커스 에러 방지용(가능하면)
			MenuWidgetInstance->SetIsFocusable(true);
			Mode.SetWidgetToFocus(MenuWidgetInstance->TakeWidget());
		}

		SetInputMode(Mode);
	}
	else
	{
		bShowMouseCursor = false;

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		FInputModeGameOnly Mode;
		SetInputMode(Mode);

		FlushPressedKeys();
	}
}
// ============================

// ============================
// [MENU 추가] : ESC 토글
// ============================
void ARSPlayerController::ToggleMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Menu] ToggleMenu called. bMenuOpen=%d"), bMenuOpen ? 1 : 0);

	if (bMenuOpen)
	{
		CloseMenu();
	}
	else
	{
		OpenMenu();
	}
}
// ============================

// ============================
// [MENU 추가] : 메뉴 열기
// ============================
void ARSPlayerController::OpenMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Menu] OpenMenu called"));

	// 게임오버가 떠있으면 메뉴 열지 않음(원하면 이 조건 제거 가능)
	if (GameOverWidget && GameOverWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Menu] GameOver is visible -> skip OpenMenu"));
		return;
	}

	// 인벤이 열려있으면 닫기 (충돌 방지)
	if (bInventoryOpen)
	{
		CloseInventoryUI();
	}

	if (!MenuWidgetInstance)
	{
		if (!MenuWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Menu] MenuWidgetClass is null (BP에서 지정 필요)"));
			return;
		}

		MenuWidgetInstance = CreateWidget<UUserWidget>(this, MenuWidgetClass);
		if (!MenuWidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Menu] Failed to CreateWidget"));
			return;
		}
	}

	if (!MenuWidgetInstance->IsInViewport())
	{
		MenuWidgetInstance->AddToViewport(200); // 다른 UI 위로 확실히
	}

	MenuWidgetInstance->SetVisibility(ESlateVisibility::Visible);

	// Pause
	SetPause(true);

	ApplyMenuInputMode(true);

	bMenuOpen = true;
	UE_LOG(LogTemp, Warning, TEXT("[Menu] OpenMenu done"));
}
// ============================

// ============================
// [MENU 추가] : 메뉴 닫기
// ============================
void ARSPlayerController::CloseMenu()
{
	if (!IsLocalController())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Menu] CloseMenu called"));

	if (MenuWidgetInstance && MenuWidgetInstance->IsInViewport())
	{
		MenuWidgetInstance->RemoveFromParent();
	}

	SetPause(false);
	ApplyMenuInputMode(false);

	bMenuOpen = false;

	UE_LOG(LogTemp, Warning, TEXT("[Menu] CloseMenu done"));
}
// ============================

void ARSPlayerController::HandleInventoryChanged()
{
	RebuildQuickPotionSlots();
	UpdateQuickSlotUI();
}

void ARSPlayerController::RebuildQuickPotionSlots()
{
	QuickPotionSlots.Reset();

	if (!InventoryComp) return;

	const TArray<FInventoryItem>& Slots = InventoryComp->GetSlots();
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		const FInventoryItem& S = Slots[i];
		if (S.IsEmpty() || !S.ItemData) continue;

		if (S.ItemData->Category == EItemCategory::Potion)
		{
			QuickPotionSlots.Add(i);
		}
	}

	if (QuickPotionSlots.Num() == 0)
	{
		QuickPotionIndex = 0;
	}
	else
	{
		QuickPotionIndex = FMath::Clamp(QuickPotionIndex, 0, QuickPotionSlots.Num() - 1);
	}
}

void ARSPlayerController::EnsureQuickSlotWidgetCreated()
{
	if (!IsLocalController() || QuickSlotWidget)
	{
		return;
	}

	if (!QuickSlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuickSlot] QuickSlotWidgetClass is not set."));
		return;
	}

	QuickSlotWidget = CreateWidget<URSQuickSlotWidget>(this, QuickSlotWidgetClass);
	if (!QuickSlotWidget)
	{
		return;
	}

	QuickSlotWidget->AddToViewport(5);
	QuickSlotWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	UE_LOG(LogTemp, Log, TEXT("[QuickSlot] Created & AddedToViewport"));
}

void ARSPlayerController::QuickSlotCycle()
{
	if (bInventoryOpen) return;
	if (QuickPotionSlots.Num() <= 1) return;

	QuickPotionIndex = (QuickPotionIndex + 1) % QuickPotionSlots.Num();
	UpdateQuickSlotUI();
}

void ARSPlayerController::QuickSlotUse()
{
	if (bInventoryOpen) return;

	APawn* P = GetPawn();
	if (!P || !InventoryComp) return;
	if (QuickPotionSlots.Num() == 0) return;

	const int32 SlotIndex = QuickPotionSlots[QuickPotionIndex];
	InventoryComp->UseItem(SlotIndex, P);
}

void ARSPlayerController::UpdateQuickSlotUI()
{
	if (!QuickSlotWidget || !InventoryComp)
	{
		return;
	}

	if (QuickPotionSlots.Num() == 0)
	{
		QuickSlotWidget->SetEmpty();
		return;
	}

	const int32 SlotIndex = QuickPotionSlots[QuickPotionIndex];
	const FInventoryItem& Item = InventoryComp->GetSlots()[SlotIndex];
	QuickSlotWidget->SetItem(Item);
}
