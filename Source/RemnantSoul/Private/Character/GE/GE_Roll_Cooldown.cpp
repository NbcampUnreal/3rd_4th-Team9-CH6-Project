// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GE/GE_Roll_Cooldown.h"
#include "GameplayTagContainer.h"


UGE_Roll_Cooldown::UGE_Roll_Cooldown()
{

	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(1.0f);
	//BP에서 내부 쿨타임 사용 용도

	InheritableOwnedTagsContainer.AddTag(
	FGameplayTag::RequestGameplayTag(TEXT("Roll.Cooldown")));
}
