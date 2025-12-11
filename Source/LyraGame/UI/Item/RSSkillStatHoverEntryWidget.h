#pragma once

#include "Blueprint/UserWidget.h"
#include "RSSkillStatHoverEntryWidget.generated.h"

class URichTextBlock;
class UTextBlock;

UCLASS()
class URSSkillStatHoverEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSSkillStatHoverEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(FText Name, FText Description);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> Text_Description;
	
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_FadeIn;
};
