#pragma once

#include "Blueprint/UserWidget.h"
#include "RSHoverWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;

UCLASS()
class URSHoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetPosition(const FVector2D& AbsolutePosition);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Hovers;
};
