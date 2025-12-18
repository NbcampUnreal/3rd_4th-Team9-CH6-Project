#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSEnemyBasicAttributeSet.generated.h"

#define RS_ENEMY_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class REMNANTSOUL_API URSEnemyBasicAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	URSEnemyBasicAttributeSet();
	
	/* Health Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBasicAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBasicAttributeSet, MaxHealth);

	/* Stamina Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stamina;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBasicAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStamina;
	RS_ENEMY_ATTRIBUTE_ACCESSORS(URSEnemyBasicAttributeSet, MaxStamina);
};
