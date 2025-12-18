#include "UI/RSUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"

void URSUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* URSUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}