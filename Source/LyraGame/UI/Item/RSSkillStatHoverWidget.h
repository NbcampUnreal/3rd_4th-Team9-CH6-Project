#pragma once

#include "UI/RSHoverWidget.h"
#include "RSSkillStatHoverWidget.generated.h"

class URSSkillStatHoverEntryWidget;

UCLASS()
class URSSkillStatHoverWidget : public URSHoverWidget
{
	GENERATED_BODY()
	
public:
	URSSkillStatHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(FText Name, FText Description);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSSkillStatHoverEntryWidget> Widget_HoverEntry;
};
