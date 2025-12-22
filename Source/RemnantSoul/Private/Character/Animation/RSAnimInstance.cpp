// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/RSAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

void URSAnimInstance::SetRollAngle(float InAngle)
{
	RollAngle = InAngle;
}

void URSAnimInstance::SetIsRolling(bool bInRolling)
{
	bIsRolling = bInRolling;
}