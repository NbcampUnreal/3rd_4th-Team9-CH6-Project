#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSEnemyBaseAttributeSet.generated.h"

#define RS_ENEMY_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class REMNANTSOUL_API URSEnemyBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	URSEnemyBaseAttributeSet();
	
	/* Health Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBaseAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBaseAttributeSet, MaxHealth);

	/* Stamina Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBaseAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBaseAttributeSet, MaxStamina);

public:
	/* Attributes의 값을 변경하기 직전에 호출되는 콜백 함수 */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/* Attributes의 값이 GameplayEffect에 의해 변경될 때 호출되는 콜백 함수 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
