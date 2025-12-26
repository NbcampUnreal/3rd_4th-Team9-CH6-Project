//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "GAS/AS/RSAbilitySet.h"
//



// Private/GAS/AS/RSAbilitySet.cpp

#include "GAS/AS/RSAbilitySet.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSAbilitySet)

void FRSAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	// 부여된 Ability 제거
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}
	AbilitySpecHandles.Reset();

	// 부여된 GameplayEffect 제거
	for (const FActiveGameplayEffectHandle& GEHandle : GameplayEffectHandles)
	{
		if (GEHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(GEHandle);
		}
	}
	GameplayEffectHandles.Reset();

	// 부여된 AttributeSet 제거
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		if (Set)
		{
			ASC->RemoveSpawnedAttribute(Set);
		}
	}
	GrantedAttributeSets.Reset();
}

URSAbilitySet::URSAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSAbilitySet::GiveToAbilitySystem(
	UAbilitySystemComponent* ASC,
	FRSAbilitySet_GrantedHandles* OutGrantedHandles,
	UObject* SourceObject
) const
{
	if (!ASC)
	{
		return;
	}

	// 1) Ability 부여
	for (const FRSAbilitySet_GameplayAbility& AbilityDef : GrantedAbilities)
	{
		if (!AbilityDef.Ability)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityDef.Ability, AbilityDef.AbilityLevel);
		Spec.SourceObject = SourceObject;

		// InputTag가 있으면 AbilitySpec의 DynamicAbilityTags에 추가해두면,
		// 나중에 ASC/GA 쪽에서 InputTag 기준으로 검색/분기가 가능하다.
		if (AbilityDef.InputTag.IsValid())
		{
			Spec.DynamicAbilityTags.AddTag(AbilityDef.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(Handle);
		}
	}

	// 2) (확장용) GameplayEffect 부여
	//    나중에 GrantedEffects 배열을 활성화하면 여기서 처리:
	/*
	for (TSubclassOf<UGameplayEffect> EffectClass : GrantedEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(SourceObject);

		const FGameplayEffectSpecHandle SpecHandle =
			ASC->MakeOutgoingSpec(EffectClass, 1.f, Context);

		if (SpecHandle.IsValid())
		{
			const FActiveGameplayEffectHandle GEHandle =
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddGameplayEffectHandle(GEHandle);
			}
		}
	}
	*/

	// 3) (확장용) AttributeSet 추가
	/*
	for (TSubclassOf<UAttributeSet> SetClass : GrantedAttributeSets)
	{
		if (!SetClass)
		{
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetClass);
		ASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
	*/
}
