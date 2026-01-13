// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSAttributeSet_Stamina.generated.h"

#define RS_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxStaminaChanged, float, OldValue, float, NewValue);

UCLASS()
class REMNANTSOUL_API URSAttributeSet_Stamina : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URSAttributeSet_Stamina();

	// UAttributeSet
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	// Stamina
	UPROPERTY(BlueprintReadOnly, Category = "RS|Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet_Stamina, Stamina)

		UPROPERTY(BlueprintReadOnly, Category = "RS|Stamina", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet_Stamina, MaxStamina)

		// Regen (선택)
		UPROPERTY(BlueprintReadOnly, Category = "RS|Stamina")
	FGameplayAttributeData StaminaRegenRate;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet_Stamina, StaminaRegenRate)

		// (선택) 스태미나 소비 후 리젠 재개까지 딜레이
		UPROPERTY(BlueprintReadOnly, Category = "RS|Stamina")
	FGameplayAttributeData StaminaRegenDelay;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet_Stamina, StaminaRegenDelay)

public:
	// UI/상태 갱신용 이벤트(원하면 사용)
	mutable FOnStaminaChanged OnStaminaChanged;
	mutable FOnMaxStaminaChanged OnMaxStaminaChanged;

protected:
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const;

private:
	void ClampStamina();
	void ClampMaxStamina();
	void HandleRegenDelayOnSpend(const FGameplayEffectModCallbackData& Data);

	// 리젠 딜레이를 태그로 막을 때 사용할 수 있게 훅만 마련
	void TryAddRegenBlockTag(UAbilitySystemComponent* ASC) const;
	void TryRemoveRegenBlockTag(UAbilitySystemComponent* ASC) const;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 내부 상태: 리젠 차단이 걸렸는지(선택)
	mutable bool bRegenBlocked = false;
};
