#pragma once

#include "RSNumberPopWidgetBase.h"
#include "RSNumberPopWidgetDefault.generated.h"

class UTextBlock;

UCLASS()
class URSNumberPopWidgetDefault : public URSNumberPopWidgetBase
{
	GENERATED_BODY()
	
public:
	URSNumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Number;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_NumberPop;
};
