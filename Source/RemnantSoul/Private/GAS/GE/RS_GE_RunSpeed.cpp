// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/RS_GE_RunSpeed.h"
#include "Character/Ability/RSAttributeSet.h" //추후 적합한 AttributeSet으로 수정 필요
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

URS_GE_RunSpeed::URS_GE_RunSpeed()
{

	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URSAttributeSet::GetRunSpeedBonusAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(360.f);

	Modifiers.Add(Modifier);
	UE_LOG(LogTemp, Warning, TEXT("[GE_RunSpeed] Constructor Called | Duration=Infinite | Additive Bonus=360"));

}


