#include "GAS/AS/RSAttributeSet_Character.h"
#include "RemnantSoul.h"
#include "GameplayEffectExtension.h"
#include "RSGameplayTags.h"

URSAttributeSet_Character::URSAttributeSet_Character()
	: AttackRange(100.0f)
	, MaxAttackRange(300.0f)
	, AttackRadius(50.f)
	, MaxAttackRadius(150.0f)
	, AttackDamage(30.0f)
	, MaxAttackDamage(100.0f)
	, Health(100.0f)
	, MaxHealth(100.0f)
	, MetaDamage(0.0f)
{
	InitHealth(GetMaxHealth());
}

void URSAttributeSet_Character::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetMetaDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

void URSAttributeSet_Character::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("Health : %.1f -> %.1f"), OldValue, NewValue);
	}
}

void URSAttributeSet_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("Direct Health Access : %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMetaDamageAttribute())
	{
		UE_LOG(LogTemp, Log, TEXT("MetaDamage : %.1f"), GetMetaDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetMetaDamage(), MinimumHealth, GetMaxHealth()));
		SetMetaDamage(0.0f);
	}

	if ((GetHealth() <= 0.0f) && bOutOfHealth == false)
	{
		Data.Target.AddLooseGameplayTag(FRSGameplayTags::Get().State_IsDead);
		OnOutOfHealth.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

bool URSAttributeSet_Character::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetMetaDamageAttribute() && KINDA_SMALL_NUMBER <= Data.EvaluatedData.Magnitude)
	{
		if (Data.Target.HasMatchingGameplayTag(FRSGameplayTags::Get().State_Invincible) == true)
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
	}

	return true;
}