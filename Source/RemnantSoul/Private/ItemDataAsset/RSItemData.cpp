// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset/RSItemData.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

bool URSItemData::TryUse(AActor* User) const
{
	if (!User)
	{
		return false;
	}

	// 포션/소모품은 보통 GE로 처리
	if (UseGameplayEffect)
	{
		return ApplyGameplayEffectToUser(User);
	}

	// GE가 없으면 현재는 사용 불가(나중에 확장 가능)
	return false;
}

bool URSItemData::ApplyGameplayEffectToUser(AActor* User) const
{
	if (!UseGameplayEffect)
	{
		return false;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(User);
	if (!ASI)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(UseGameplayEffect, 1.0f, Context);
	if (!Spec.IsValid())
	{
		return false;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	return true;
}
