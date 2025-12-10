#pragma once

#include "Blueprint/UserWidget.h"
#include "RSInventorySlotWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class URSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSInventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
