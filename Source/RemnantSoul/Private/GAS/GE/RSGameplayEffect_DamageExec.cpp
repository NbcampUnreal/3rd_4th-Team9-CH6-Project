#include "GAS/GE/RSGameplayEffect_DamageExec.h"
#include "GAS/GE/CustomCalculation/RSCustomExecution_Damage.h"

URSGameplayEffect_DamageExec::URSGameplayEffect_DamageExec()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition ExecDef;
	ExecDef.CalculationClass = URSCustomExecution_Damage::StaticClass();

	Executions.Add(ExecDef);

	// Modifier는 여기서 Health에 직접 박지 않는다.
	// 결과는 ExecCalc가 MetaDamage로 낸다.
}
