#include "GAS/GE/RSGameplayEffect_Damage.h"

#include "GAS/AS/RSAttributeSet_Character.h"

URSGameplayEffect_Damage::URSGameplayEffect_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(URSAttributeSet_Character::StaticClass(), GET_MEMBER_NAME_CHECKED(URSAttributeSet_Character, Health)));
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FScalableFloat DamageAmount(-30.0f);
	FGameplayEffectModifierMagnitude ModMagnitude(DamageAmount);

	HealthModifier.ModifierMagnitude = ModMagnitude;
	Modifiers.Add(HealthModifier);
}