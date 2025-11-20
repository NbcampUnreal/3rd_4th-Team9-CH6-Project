#include "Game/AbilitySystem/CVAbilitySet.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"

void FCVAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FCVAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FCVAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if (Set)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FCVAbilitySet_GrantedHandles::TakeFromAbilitySystem(UCVAbilitySystemComponent* CVASC)
{
	check(CVASC);

	if (!CVASC->IsOwnerActorAuthoritative())
	{
		// 권한이 있어야만(Authoritative) Ability를 주거나 회수할 수 있다.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			CVASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			CVASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		if (AttributeSet)
		{
			CVASC->RemoveSpawnedAttribute(AttributeSet);
		}
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UCVAbilitySet::UCVAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCVAbilitySet::GiveToAbilitySystem(UCVAbilitySystemComponent* CVASC, FCVAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(CVASC);

	if (!CVASC->IsOwnerActorAuthoritative())
	{
		// 권한이 있어야만(Authoritative) Ability를 주거나 회수할 수 있다.
		return;
	}

	GrantAttributeSets(CVASC, OutGrantedHandles);
	GrantAbilities(CVASC, OutGrantedHandles, SourceObject);
	GrantGameplayEffects(CVASC, OutGrantedHandles);
}

void UCVAbilitySet::GrantAttributeSets(UCVAbilitySystemComponent* CVASC, FCVAbilitySet_GrantedHandles* OutGrantedHandles) const
{
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); SetIndex++)
	{
		const FCVAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		AActor* ASCOwner = CVASC->GetOwner();
		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASCOwner, SetToGrant.AttributeSet);
		CVASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

void UCVAbilitySet::GrantAbilities(UCVAbilitySystemComponent* CVASC, FCVAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); AbilityIndex++)
	{
		const FCVAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = CVASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}


void UCVAbilitySet::GrantGameplayEffects(UCVAbilitySystemComponent* CVASC, FCVAbilitySet_GrantedHandles* OutGrantedHandles) const
{
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FCVAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FGameplayEffectContextHandle ContextHandle = CVASC->MakeEffectContext();
		const FActiveGameplayEffectHandle GameplayEffectHandle = CVASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ContextHandle);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}

