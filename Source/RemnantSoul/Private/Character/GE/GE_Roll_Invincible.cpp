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
	//TODO::추후 이 상태일 시 일부 오브젝트를 관통할 수 있는 코드로 확장.
	//향후 개발 과정에서 AI 적의 무기가 대검일 경우 구르기 도중 적 무기에 막히지 않도록 구현 가능한지 확인 필요.
	//+ 캐릭터 AI 불문하고 데미지를 부여하는 구간에서 Invincible.Roll 태그가 있을 경우 어빌리티에서 데미지 부여 GE를 호출하지 않는 방향으로 작성해야함.
}
