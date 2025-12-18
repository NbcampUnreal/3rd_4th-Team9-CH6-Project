// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GE/GE_Roll_Cost.h"
#include "Character/Ability/RSAttributeSet.h"

UGE_Roll_Cost::UGE_Roll_Cost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URSAttributeSet::GetStaminaAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(-20.f);
	//추후 스테미나 소모량 조절은 BP에서 가능, 스테미너 회복은 별도로 구현해야할 듯.

	Modifiers.Add(Modifier);
}