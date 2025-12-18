

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

		if (OldHealth > 0.f && NewHealth <= 0.f)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[AttributeSet] Health <= 0 detected. Calling HandleOutOfHealth()")
			);

			HandleOutOfHealth(Data);
		}
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

void URSAttributeSet::HandleOutOfHealth(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Error,
		TEXT("[AttributeSet] HandleOutOfHealth() called")
	);

	AActor* OwnerActor = GetOwningActor();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AttributeSet] OwnerActor is NULL")
		);
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[AttributeSet] OwnerActor: %s"),
		*OwnerActor->GetName()
	);

	if (ARSBaseCharacter* Player = Cast<ARSBaseCharacter>(OwnerActor))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AttributeSet] OwnerActor cast to ARSBaseCharacter SUCCESS")
		);

		Player->HandleDeath();
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("[AttributeSet] OwnerActor cast to ARSBaseCharacter FAILED")
		);
	}
}
