

#include "Character/Ability/RSAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/RSBaseCharacter.h"


URSAttributeSet::URSAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
	InitAttackDamage(10.f);

	InitRollCooldown(0.f);
	InitMaxRollCooldown(1.0f); //GE에서 값을 덮어쓸 수 있음.
}

void URSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Health 보정
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float OldHealth =
			GetHealth() - Data.EvaluatedData.Magnitude;

		const float NewHealth = FMath::Clamp(
			GetHealth(),
			0.f,
			GetMaxHealth()
		);

		SetHealth(NewHealth);

		UE_LOG(LogTemp, Warning,
			TEXT("[AttributeSet] Health Changed | Old: %.2f -> New: %.2f"),
			OldHealth,
			NewHealth
		);


	}


	// Stamina 보정
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		const float NewStamina = FMath::Clamp(
			GetStamina(),
			0.f,
			GetMaxStamina()
		);

		SetStamina(NewStamina);
	}
}