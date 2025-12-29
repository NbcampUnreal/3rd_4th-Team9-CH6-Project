// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "RSPlayerController.generated.h"

class URSInventoryWidget;
class URSInventoryComponent;

UCLASS()
class REMNANTSOUL_API ARSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
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

protected:
	virtual void BeginPlay() override;
	void OnPossess(APawn* InPawn);

	// 인벤 위젯 클래스(에디터에서 WBP_Inventory 지정)
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<URSInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<URSInventoryWidget> InventoryWidget;
	
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;
	bool bInventoryWidgetInited;

private:
	bool bInventoryOpen = false;

	void EnsureInventoryWidgetCreated();
	void ApplyInventoryInputMode(bool bOpen);
	
	
};
