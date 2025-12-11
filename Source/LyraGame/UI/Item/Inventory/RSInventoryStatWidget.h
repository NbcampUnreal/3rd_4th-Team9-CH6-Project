#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "RSInventoryStatWidget.generated.h"

class URSSkillStatHoverWidget;

UCLASS()
class URSInventoryStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSInventoryStatWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeDestruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag StatTag;
	
protected:
	UPROPERTY()
	TObjectPtr<URSSkillStatHoverWidget> HoverWidget;
};
