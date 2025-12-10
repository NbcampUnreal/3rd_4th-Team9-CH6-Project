#pragma once

#include "CommonActivatableWidget.h"
#include "RSActivatableWidget.generated.h"

UCLASS()
class URSActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	URSActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	// TODO: Switch to InputAction
	UPROPERTY(EditDefaultsOnly)
	FKey DeactivateKey;
};
