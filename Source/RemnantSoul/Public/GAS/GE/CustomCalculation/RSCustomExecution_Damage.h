#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "RSCustomExecution_Damage.generated.h"

UCLASS()
class REMNANTSOUL_API URSCustomExecution_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	URSCustomExecution_Damage();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;
};
