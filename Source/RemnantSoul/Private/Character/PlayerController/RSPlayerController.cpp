// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerController/RSPlayerController.h"
#include "IngameUI/inventory/RSInventoryWidget.h"
//#include "Input/RSEnhancedInputComponent.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "GameFramework/Pawn.h"

static const float PickupRange = 250.0f;


ARSPlayerController::ARSPlayerController()
{
	bShowMouseCursor = false;
}


void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ARSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InventoryComp = InPawn ? InPawn->FindComponentByClass<URSInventoryComponent>() : nullptr;
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
