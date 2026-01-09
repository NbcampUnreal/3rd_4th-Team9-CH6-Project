#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "RSCustomCalculation_StaminaCost.generated.h"

UCLASS()
class REMNANTSOUL_API URSCustomCalculation_StaminaCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	URSCustomCalculation_StaminaCost();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
