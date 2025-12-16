#pragma once


#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSAttributeSet.generated.h"


#define RS_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class REMNANTSOUL_API URSAttributeSet : public UAttributeSet
{
GENERATED_BODY()


public:
	URSAttributeSet();
	
	//현재 체력 및 최대 체력 란.
	UPROPERTY(BlueprintReadOnly, Category="HP")
	FGameplayAttributeData Health;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category="HP")
	FGameplayAttributeData MaxHealth;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet, MaxHealth)


	//공격 데미지
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	FGameplayAttributeData AttackDamage;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet, AttackDamage)


	//스태미나 현황
	UPROPERTY(BlueprintReadOnly, Category="Stamina")
	FGameplayAttributeData Stamina;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet, Stamina)


	UPROPERTY(BlueprintReadOnly, Category="Stamina")
	FGameplayAttributeData MaxStamina;
	RS_ATTRIBUTE_ACCESSORS(URSAttributeSet, MaxStamina)

protected:
	// GameplayEffect 적용 후 Attribute 값 보정을 위해 GAS가 자동 호출하는 콜백
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	//캐릭터의 체력이 0이 되었을 경우 캐릭터 사망 처리 함수를 콜
	void HandleOutOfHealth(const FGameplayEffectModCallbackData& Data);

};