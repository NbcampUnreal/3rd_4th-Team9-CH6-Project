#pragma once

#include "UI/RSActivatableWidget.h"
#include "RSClassSelectionWidget.generated.h"

class UButton;
class UVerticalBox;
class URSClassEntryWidget;

UCLASS()
class URSClassSelectionWidget : public URSActivatableWidget
{
	GENERATED_BODY()
	
public:
	URSClassSelectionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnExitButtonClicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSClassEntryWidget> ClassEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_ClassElements;
};
