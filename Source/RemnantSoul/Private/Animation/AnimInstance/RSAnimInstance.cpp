// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstance/RSAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"

void URSAnimInstance::SetRollAngle(float InAngle)
{
	RollAngle = InAngle;
}

void URSAnimInstance::SetIsRolling(bool bInRolling)
{
	bIsRolling = bInRolling;
}


void URSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwnerActor = GetOwningActor();
	if (!OwnerActor)
	{
		return;
	}

	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor))
	{
		AbilitySystemComponent = ASI->GetAbilitySystemComponent();
	}

	// GameplayTag 캐싱
	CrouchStartTag = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.Start"));
	CrouchLoopTag  = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.Loop"));
	CrouchEndTag   = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.End"));
}

void URSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!AbilitySystemComponent)
	{
		return;
	}

	// === Crouch Tag → Bool 변환 ===
	bCrouchStart = AbilitySystemComponent->HasMatchingGameplayTag(CrouchStartTag);
	bCrouchLoop  = AbilitySystemComponent->HasMatchingGameplayTag(CrouchLoopTag);
	bCrouchEnd   = AbilitySystemComponent->HasMatchingGameplayTag(CrouchEndTag);

	//하나라도 True면 true로 인 논리 OR 연산자
	bIsCrouching = bCrouchStart || bCrouchLoop || bCrouchEnd;
	
}