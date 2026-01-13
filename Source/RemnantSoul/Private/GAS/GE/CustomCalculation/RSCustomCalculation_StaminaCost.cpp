#include "GAS/GE/CustomCalculation/RSCustomCalculation_StaminaCost.h"
#include "RSGameplayTags.h"

URSCustomCalculation_StaminaCost::URSCustomCalculation_StaminaCost()
{
}

float URSCustomCalculation_StaminaCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// SetByCaller로 받은 Cost(양수)를 읽어서, 실제 적용은 "음수(감소)"로 반환
	const float Cost = Spec.GetSetByCallerMagnitude(Tags.Data_StaminaCost, false, 0.0f);
	return -FMath::Max(Cost, 0.0f);
}
