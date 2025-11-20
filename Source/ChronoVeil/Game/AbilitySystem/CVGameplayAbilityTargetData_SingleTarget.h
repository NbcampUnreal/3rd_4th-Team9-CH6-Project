#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "CVGameplayAbilityTargetData_SingleTarget.generated.h"

USTRUCT()
struct FCVGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
public:
	FCVGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FCVGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}

	// 탄약 ID (카트리지)
	UPROPERTY()
	int32 CartridgeID;
};