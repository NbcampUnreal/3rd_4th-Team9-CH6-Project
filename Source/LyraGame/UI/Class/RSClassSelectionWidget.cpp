#include "RSClassSelectionWidget.h"

#include "RSClassEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Data/RSClassData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSClassSelectionWidget)

URSClassSelectionWidget::URSClassSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_ClassElements->ClearChildren();

	const int32 ClassCount = (int32)ECharacterClassType::Count;
	for (int i = 0; i < ClassCount; i++)
	{
		URSClassEntryWidget* ClassEntryWidget = CreateWidget<URSClassEntryWidget>(this, ClassEntryWidgetClass);
		ClassEntryWidget->InitializeUI(this, (ECharacterClassType)i);
		VerticalBox_ClassElements->AddChild(ClassEntryWidget);
	}
}

void URSClassSelectionWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
