#include "RSCheatListWidget.h"

#include "RSCheatEntryWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCheatListWidget)

URSCheatListWidget::URSCheatListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSCheatListWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void URSCheatListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox_Entries->ClearChildren();
}

void URSCheatListWidget::AddEntry(URSCheatEntryWidget* EntryWidget)
{
	ScrollBox_Entries->AddChild(EntryWidget);
}
