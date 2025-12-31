#include "UI/RSHPBarWidget.h"
#include "AbilitySystemComponent.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "RSGameplayTags.h"

void URSHPBarWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(URSAttributeSet_Character::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(URSAttributeSet_Character::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		//HPBar->SetFillColorAndOpacity(FLinearColor::Green);
		ASC->RegisterGameplayTagEvent(FRSGameplayTags::Get().State_Invincible, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ThisClass::OnInvincibleTagGranted);
		HPBar->SetFillColorAndOpacity(NormalHPBarColor);

		const URSAttributeSet_Character* CurrentAttributeSet = ASC->GetSet<URSAttributeSet_Character>();
		if (CurrentAttributeSet)
		{
			CurrentHealth = CurrentAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}
	}
}

void URSHPBarWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void URSHPBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void URSHPBarWidget::UpdateHpBar()
{
	if (HPBar)
	{
		HPBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}

	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%0.f"), CurrentHealth, CurrentMaxHealth)));
	}
}

void URSHPBarWidget::OnInvincibleTagGranted(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HPBar->SetFillColorAndOpacity(InvincibleHPBarColor);
		HPBar->SetPercent(1.0f);
	}
	else
	{
		HPBar->SetFillColorAndOpacity(NormalHPBarColor);
		UpdateHpBar();
	}
}