#pragma once

#include "Blueprint/UserWidget.h"
#include "RSNumberPopWidgetBase.generated.h"

UCLASS()
class URSNumberPopWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSNumberPopWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor);
};
