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

bool URSAttributeSet_Character::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	const bool bInv = Data.Target.HasMatchingGameplayTag(FRSGameplayTags::Get().State_Invincible);
	if (!bInv)
	{
		return true;
	}

	const bool bDamageToMeta = (Data.EvaluatedData.Attribute == GetMetaDamageAttribute());
	const bool bDamageToHealth = (Data.EvaluatedData.Attribute == GetHealthAttribute());

	// 데미지로 들어오는 "감소"만 막고 싶으면, 부호/연산까지 더 정밀하게 봐야 하지만
	// 지금 핫픽스는 "무적이면 Health/MetaDamage 변경을 0"으로 만든다.
	if ((bDamageToMeta || bDamageToHealth) && FMath::Abs(Data.EvaluatedData.Magnitude) > KINDA_SMALL_NUMBER)
	{
		Data.EvaluatedData.Magnitude = 0.0f;
		return true; // false로 끊지 말고 흘려보내는 게 부작용이 적음
	}

	return true;
}
