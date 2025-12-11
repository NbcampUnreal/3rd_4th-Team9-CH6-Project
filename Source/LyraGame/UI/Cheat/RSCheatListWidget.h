#pragma once

#include "Blueprint/UserWidget.h"
#include "RSCheatListWidget.generated.h"

class UTextBlock;
class URSCheatEntryWidget;
class UScrollBox;

UCLASS()
class URSCheatListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSCheatListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void AddEntry(URSCheatEntryWidget* EntryWidget);

protected:
	UPROPERTY(EditAnywhere)
	FText TitleText;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Entries;
};
