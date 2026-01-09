#include "UI/RSPlayerVitalsWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "GAS/AS/RSAttributeSet_Stamina.h"

void URSPlayerVitalsWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (!ASC || !PB_Stamina)
	{
		return;
	}

	PB_Stamina->SetFillColorAndOpacity(FLinearColor::Green);

	ASC->GetGameplayAttributeValueChangeDelegate(URSAttributeSet_Stamina::GetStaminaAttribute())
		.AddUObject(this, &ThisClass::OnStaminaChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(URSAttributeSet_Stamina::GetMaxStaminaAttribute())
		.AddUObject(this, &ThisClass::OnMaxStaminaChanged);

	if (const URSAttributeSet_Stamina* Set = ASC->GetSet<URSAttributeSet_Stamina>())
	{
		CurrentStamina = Set->GetStamina();
		CurrentMaxStamina = FMath::Max(Set->GetMaxStamina(), 1.f);
		UpdateStaminaBar();
	}
}

void URSPlayerVitalsWidget::OnStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentStamina = ChangeData.NewValue;
	UpdateStaminaBar();
}

void URSPlayerVitalsWidget::OnMaxStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxStamina = FMath::Max(ChangeData.NewValue, 1.f);
	UpdateStaminaBar();
}

void URSPlayerVitalsWidget::UpdateStaminaBar()
{
	if (!PB_Stamina)
	{
		return;
	}

	PB_Stamina->SetPercent(CurrentStamina / CurrentMaxStamina);
}
