// RSQuickSlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/Inventory/RSInventoryType.h"
#include "RSQuickSlotWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class REMNANTSOUL_API URSQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(const FInventoryItem& InItem);
	void SetEmpty();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CountText;
};
