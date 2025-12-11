#pragma once

#include "RSDefine.h"
#include "Blueprint/UserWidget.h"
#include "RSInventoryValidWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class URSInventoryValidWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSInventoryValidWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	void ChangeSlotState(ESlotState SlotState);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;
};
