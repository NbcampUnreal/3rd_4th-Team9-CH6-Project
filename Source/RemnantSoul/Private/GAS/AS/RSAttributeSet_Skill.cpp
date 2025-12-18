#include "GAS/AS/RSAttributeSet_Skill.h"

URSAttributeSet_Skill::URSAttributeSet_Skill()
	: SkillRadius(800.0f)
	, MaxSkillRadius(1200.0f)
	, SkillAttackDamage(150.0f)
	, MaxSkillAttackDamage(300.0f)
	, SkillEnergy(100.0f)
	, MaxSkillEnergy(100.0f)
{
}

void URSAttributeSet_Skill::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetSkillRadiusAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, GetMaxSkillRadius());
	}
	else if (Attribute == GetSkillAttackDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSkillAttackDamage());
	}
}
