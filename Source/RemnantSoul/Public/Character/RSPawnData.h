#pragma once

#include "GAS/AS/RSAbilitySet.h"
#include "RSPawnData.generated.h"

class URSInputConfig;
class URSAbilitySet;

class UDataAsset;

UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSPawnData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Pawn의 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnData|Input")
	TObjectPtr<URSInputConfig> InputConfig = nullptr;

	// Pawn에 기본으로 깔릴 AbilitySet들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnData|Abilities")
	TArray<TObjectPtr<URSAbilitySet>> AbilitySets;
};
