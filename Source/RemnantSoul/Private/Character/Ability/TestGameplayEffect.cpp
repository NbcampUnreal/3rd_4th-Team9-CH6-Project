// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/TestGameplayEffect.h"
#include "Character/Ability/RSAttributeSet.h"

UTestGameplayEffect::UTestGameplayEffect()
{
	// 즉시 적용
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Modifier 하나 추가
	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URSAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.f));

	Modifiers.Add(Modifier);
	
}