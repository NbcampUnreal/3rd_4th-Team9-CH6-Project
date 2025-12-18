#include "Component/RSWidgetComponent.h"
#include "UI/RSUserWidget.h"

void URSWidgetComponent::InitWidget()
{
	Super::InitWidget();

	URSUserWidget* RSUserWidget = Cast<URSUserWidget>(GetWidget());
	if (RSUserWidget)
	{
		RSUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}
