#pragma once

#include "GAS/TA/RSTargetActor_SweepSingleCapsule.h"
#include "RSTargetActor_OverlapMultiSphere.generated.h"

UCLASS()
class REMNANTSOUL_API ARSTargetActor_OverlapMultiSphere : public ARSTargetActor_SweepSingleCapsule
{
	GENERATED_BODY()

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const override;

};
