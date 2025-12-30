// RSInventoryWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSInventoryWidget.generated.h"

class UPanelWidget;
class URSInventoryComponent;
class URSInventorySlotWidget;

UCLASS()
class REMNANTSOUL_API URSInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Refresh();

	void Init(URSInventoryComponent* InInventory);

	// 기존 함수 유지: 이제 내부에서 자동생성도 수행
	void CollectSlots();

	// 추가: 슬롯 자동 생성/배치
	void BuildSlots();

protected:
	// BP에서 UniformGridPanel(권장) 또는 다른 PanelWidget로 바인딩
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPanelWidget> SlotContainer;

	UPROPERTY()
	TArray<URSInventorySlotWidget*> SlotWidgets;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<URSInventorySlotWidget> SlotWidgetClass;

	// 추가: 그리드 레이아웃 설정
	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	int32 Columns = 5;

	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	int32 Rows = 10;

	// true면 자식 슬롯이 없을 때 자동 생성
	UPROPERTY(EditAnywhere, Category="Inventory|Layout")
	bool bAutoBuildSlots = true;

private:
	UPROPERTY()
	TObjectPtr<URSInventoryComponent> InventoryComp;
};