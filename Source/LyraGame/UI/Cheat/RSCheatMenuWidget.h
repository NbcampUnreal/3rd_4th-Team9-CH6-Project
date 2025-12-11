#pragma once

#include "UI/RSActivatableWidget.h"
#include "RSCheatMenuWidget.generated.h"

class UButton;
class URSCheatEntryWidget;
class URSCheatListWidget;

UCLASS()
class URSCheatMenuWidget : public URSActivatableWidget
{
	GENERATED_BODY()
	
public:
	URSCheatMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnExitButtonClicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSCheatEntryWidget> CheatEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSCheatListWidget> CheatList_PrimaryWeapon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSCheatListWidget> CheatList_SecondaryWeapon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSCheatListWidget> CheatList_Utility;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSCheatListWidget> CheatList_Armor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSCheatListWidget> CheatList_Animation;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Exit;
};
