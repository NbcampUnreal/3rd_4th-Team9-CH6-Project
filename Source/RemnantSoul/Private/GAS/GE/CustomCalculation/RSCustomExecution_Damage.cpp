#include "GAS/GE/CustomCalculation/RSCustomExecution_Damage.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "RSGameplayTags.h"

URSCustomExecution_Damage::URSCustomExecution_Damage()
{

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

	// 1) BaseDamage는 SetByCaller (수치 규칙 고정)
	const float BaseDamage = Spec.GetSetByCallerMagnitude(Tags.Data_Damage, false, 0.f);
	if (BaseDamage <= 0.f)
	{
		return;
	}

	// 2) 태그 판정은 "MergedTags"로만 한다. (전달 경로 안정화)
	FGameplayTagContainer MergedTags;

	if (const FGameplayTagContainer* Src = Spec.CapturedSourceTags.GetAggregatedTags())
	{
		MergedTags.AppendTags(*Src);
	}
	if (const FGameplayTagContainer* Tgt = Spec.CapturedTargetTags.GetAggregatedTags())
	{
		MergedTags.AppendTags(*Tgt);
	}

	// Spec에 직접 부여한 동적 태그(GA가 넣는 판정 태그)
	// UE 버전에 따라 접근자가 다를 수 있다.
	// 네 프로젝트가 컴파일되는 쪽을 남기고, 나머지는 주석 처리해라.

	// (A) 멤버로 접근 가능할 때
	MergedTags.AppendTags(Spec.DynamicGrantedTags);
	MergedTags.AppendTags(Spec.DynamicAssetTags);

	// (B) 접근자가 있을 때(프로젝트에서 이게 맞으면 A를 지우고 이걸 써라)
	// MergedTags.AppendTags(Spec.GetDynamicGrantedTags());
	// MergedTags.AppendTags(Spec.GetDynamicAssetTags());

	// 3) 배율 결정(기존 로직 유지)
	float Multiplier = 1.0f;

	if (MergedTags.HasTagExact(Tags.Damage_Type_Light))
	{
		Multiplier *= 1.0f;
	}
	if (MergedTags.HasTagExact(Tags.Damage_Type_Heavy))
	{
		Multiplier *= 1.4f;
	}

	if (MergedTags.HasTagExact(Tags.Weapon_Sword_OneHand))
	{
		Multiplier *= 1.10f;
	}
	if (MergedTags.HasTagExact(Tags.Weapon_Staff_TwoHand))
	{
		Multiplier *= 1.25f;
	}

	const float FinalDamage = BaseDamage * Multiplier;

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			URSAttributeSet_Character::GetMetaDamageAttribute(),
			EGameplayModOp::Additive,
			FinalDamage
		)
	);
}
