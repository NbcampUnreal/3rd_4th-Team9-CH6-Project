// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "RSPlayerController.generated.h"

class URSInventoryWidget;
class URSInventoryComponent;
class URSEnhancedInputComponent;
class URSQuickSlotWidget;
class URSGameOverWidget;

UCLASS()
class REMNANTSOUL_API ARSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UseItemFromSlot(int32 SlotIndex);
	
	ARSPlayerController();
	// 인벤 UI
	UFUNCTION(BlueprintCallable)
	void ToggleInventoryUI();

	UFUNCTION(BlueprintCallable)
	void OpenInventoryUI();

	UFUNCTION(BlueprintCallable)
	void CloseInventoryUI();

	bool IsInventoryOpen() const { return bInventoryOpen; }
	
	void OnPlayerDeath();
	
	virtual void Tick(float DeltaSeconds) override;

	void ShowGameOverUI();
	
	void QuickSlotCycle();
	void QuickSlotUse();   

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// 인벤 위젯 클래스(에디터에서 WBP_Inventory 지정)
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<URSInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<URSInventoryWidget> InventoryWidget;
	
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;
	bool bInventoryWidgetInited;
	
	
	
	UPROPERTY(EditDefaultsOnly, Category="UI|QuickSlot")
	TSubclassOf<URSQuickSlotWidget> QuickSlotWidgetClass;

	UPROPERTY()
	TObjectPtr<URSQuickSlotWidget> QuickSlotWidget;

	// “포션이 들어있는 인벤 슬롯 인덱스들”
	UPROPERTY(Transient)
	TArray<int32> QuickPotionSlots;

	UPROPERTY(Transient)
	int32 QuickPotionIndex = 0;

	void HandleInventoryChanged();
	void RebuildQuickPotionSlots();
	void UpdateQuickSlotUI();
	
	
	// 에디터에서 WBP_GameOver를 할당할 변수
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> GameOverWidget;

private:
	bool bInventoryOpen = false;

	void EnsureInventoryWidgetCreated();
	void ApplyInventoryInputMode(bool bOpen);
	// QuickSlot
	void EnsureQuickSlotWidgetCreated();
	
	
};
