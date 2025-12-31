#include "GAS/GE/CustomCalculation/RSCustomExecution_Damage.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "RSGameplayTags.h"

URSCustomExecution_Damage::URSCustomExecution_Damage()
{
	// 필요 시 Attribute 캡쳐 정의를 여기에 추가 가능 (지금은 SetByCaller 중심이므로 생략)
}

void URSCustomExecution_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!IsValid(SourceASC) || !IsValid(TargetASC))
	{
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// 1) GA가 넣어준 “기본 데미지 BaseDamage”만 읽는다.
	const float BaseDamage = Spec.GetSetByCallerMagnitude(Tags.Data_Damage, false, 0.f);
	if (BaseDamage <= 0.f)
	{
		return;
	}

	// 2) 태그 기반 배율 결정
	// - Spec.CapturedSourceTags / CapturedTargetTags 사용
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Multiplier = 1.0f;

	// 라이트/헤비 배율
	if (SourceTags && SourceTags->HasTagExact(Tags.Damage_Type_Light))
	{
		Multiplier *= 1.0f;     // 라이트 1.0
	}
	if (SourceTags && SourceTags->HasTagExact(Tags.Damage_Type_Heavy))
	{
		Multiplier *= 1.4f;     // 헤비 1.4 (예시)
	}

	// 무기 타입 배율 (예시)
	if (SourceTags && SourceTags->HasTagExact(Tags.Weapon_Sword_OneHand))
	{
		Multiplier *= 1.10f;
	}
	if (SourceTags && SourceTags->HasTagExact(Tags.Weapon_Staff_TwoHand))
	{
		Multiplier *= 1.25f;
	}

	// 3) 최종 데미지
	const float FinalDamage = BaseDamage * Multiplier;

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			URSAttributeSet_Character::GetMetaDamageAttribute(),
			EGameplayModOp::Additive,
			FinalDamage
		)
	);
}
