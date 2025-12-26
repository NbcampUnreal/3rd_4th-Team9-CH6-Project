// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/RS_GE_RunSpeedDecay.h"
#include "Character/Ability/RSAttributeSet.h"

URS_GE_RunSpeedDecay::URS_GE_RunSpeedDecay()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(0.5f);

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URSAttributeSet::GetRunSpeedBonusAttribute();
	Modifier.ModifierOp = EGameplayModOp::Override;

	// ⚠ 이 값은 BP에서 Curve로 덮는 걸 전제로 함
	Modifier.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Modifier);

	UE_LOG(LogTemp, Warning,TEXT("[GE_RunSpeedDecay] Constructor Called | Duration=0.3 | Override Bonus=0"));

}