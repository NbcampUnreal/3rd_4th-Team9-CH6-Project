#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSAttributeSet_Character.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
// C++에서 클래스를 만들고 게임 구조를 짜다보면 게터/세터를 정말 자주 정의함. 어트리뷰트 셋도 마찬가지. 
// 애시당초 어트리뷰트 셋에서는 매크로를 통해 게터/세터 함수가 자동으로 작성되게끔 함.

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfHealthDelegate);

UCLASS()
class REMNANTSOUL_API URSAttributeSet_Character : public UAttributeSet
{
	GENERATED_BODY()

	friend class URSGameplayEffect_Damage;

public:
	URSAttributeSet_Character();

	ATTRIBUTE_ACCESSORS(ThisClass, AttackRange);

	ATTRIBUTE_ACCESSORS(ThisClass, MaxAttackRange);

	ATTRIBUTE_ACCESSORS(ThisClass, AttackRadius);

	ATTRIBUTE_ACCESSORS(ThisClass, MaxAttackRadius);

	ATTRIBUTE_ACCESSORS(ThisClass, AttackDamage);

	ATTRIBUTE_ACCESSORS(ThisClass, MaxAttackDamage);

	ATTRIBUTE_ACCESSORS(ThisClass, Health);

	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);

	ATTRIBUTE_ACCESSORS(ThisClass, MetaDamage);


public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;

	mutable FOnOutOfHealthDelegate OnOutOfHealth;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MetaDamage;

	bool bOutOfHealth = false;
};

