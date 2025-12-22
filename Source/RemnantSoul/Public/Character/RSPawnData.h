#pragma once

#include "Engine/DataAsset.h"
#include "RSPawnData.generated.h"

class URSInputConfig;

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
	TArray<TObjectPtr<UDataAsset>> AbilitySets;
};
