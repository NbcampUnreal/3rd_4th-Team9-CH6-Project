#pragma once

#include "Blueprint/UserWidget.h"
#include "RSInteractionWidget.generated.h"

UCLASS()
class URSInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSInteractionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
