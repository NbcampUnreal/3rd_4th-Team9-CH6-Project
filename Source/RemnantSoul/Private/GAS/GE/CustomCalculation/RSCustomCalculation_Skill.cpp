#include "GAS/GE/CustomCalculation/RSCustomCalculation_Skill.h"
#include "AbilitySystemComponent.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "GAS/AS/RSAttributeSet_Skill.h"

void URSCustomCalculation_Skill::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (SourceASC && TargetASC)
	{
		AActor* SourceActor = SourceASC->GetAvatarActor();
		AActor* TargetActor = TargetASC->GetAvatarActor();

		if (SourceActor && TargetActor)
		{
			const float MaxDamageRange = SourceASC->GetNumericAttributeBase(URSAttributeSet_Skill::GetSkillRadiusAttribute());
			const float MaxDamage = SourceASC->GetNumericAttributeBase(URSAttributeSet_Skill::GetSkillAttackDamageAttribute());
			const float Distance = FMath::Clamp(SourceActor->GetDistanceTo(TargetActor), 0.0f, MaxDamageRange);
			const float InvDamageRatio = 1.0f - Distance / MaxDamageRange;
			float Damage = InvDamageRatio * MaxDamage;

			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URSAttributeSet_Character::GetMetaDamageAttribute(), EGameplayModOp::Additive, Damage));
		}
	}
}
