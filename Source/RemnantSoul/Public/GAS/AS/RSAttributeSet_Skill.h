#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RSAttributeSet_Skill.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class REMNANTSOUL_API URSAttributeSet_Skill : public UAttributeSet
{
	GENERATED_BODY()

public:
	URSAttributeSet_Skill();

	ATTRIBUTE_ACCESSORS(ThisClass, SkillRadius);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxSkillRadius);
	ATTRIBUTE_ACCESSORS(ThisClass, SkillAttackDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxSkillAttackDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, SkillEnergy);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxSkillEnergy);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillRadius;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillRadius;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillAttackDamage;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillEnergy;

	UPROPERTY(BlueprintReadOnly, Category = "URSAttributeSet_Skill|Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillEnergy;

};
