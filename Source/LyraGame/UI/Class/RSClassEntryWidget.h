#pragma once

#include "RSDefine.h"
#include "Blueprint/UserWidget.h"
#include "RSClassEntryWidget.generated.h"

class UImage;
class UButton;
class UTextBlock;
class UVerticalBox;
class URSClassSelectionWidget;
class URSClassSkillEntryWidget;

UCLASS()
class URSClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSClassEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION()
	void InitializeUI(URSClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType);

private:
	UFUNCTION()
	void OnButtonClicked();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSClassSkillEntryWidget> SkillEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Class;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ClassName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_SkillElements;

private:
	UPROPERTY()
	ECharacterClassType CachedClassType = ECharacterClassType::Count;

	UPROPERTY()
	TWeakObjectPtr<URSClassSelectionWidget> CachedOwnerWidget;
};
