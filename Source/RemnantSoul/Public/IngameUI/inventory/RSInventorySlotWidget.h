// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class URSItemData;

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetData(URSItemData* InItemData, int32 InCount);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CountText;

private:
	UPROPERTY()
	TObjectPtr<URSItemData> ItemData;

	int32 Count = 0;
};
