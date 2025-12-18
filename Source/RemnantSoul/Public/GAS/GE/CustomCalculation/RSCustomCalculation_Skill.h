#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "RSCustomCalculation_Skill.generated.h"

UCLASS()
class REMNANTSOUL_API URSCustomCalculation_Skill : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
