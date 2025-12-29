// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GE/GE_Roll_Invincible.h"
#include "GameplayTagContainer.h"


UGE_Roll_Invincible::UGE_Roll_Invincible()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(0.5f);
	//BP에서 무적 시간 조정 가능.

	InheritableBlockedAbilityTagsContainer.AddTag(
		FGameplayTag::RequestGameplayTag(TEXT("Roll.Invincible")));
}
