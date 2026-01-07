#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSPuzzleDefinition.generated.h"

UENUM(BlueprintType)
enum class ERSPuzzleRule : uint8
{
	MatchAllStates UMETA(DisplayName="Match All States"), // 모든 노드 상태가 목표와 같으면 성공
};

USTRUCT(BlueprintType)
struct FRSPuzzleNodeTarget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag NodeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TargetState = 0;
};

UCLASS(BlueprintType)
class REMNANTSOUL_API URSPuzzleDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	FGameplayTag PuzzleTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	ERSPuzzleRule Rule = ERSPuzzleRule::MatchAllStates;

	// 노드별 목표 상태
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	TArray<FRSPuzzleNodeTarget> Targets;

	// 각 노드의 상태 범위(기본 0..MaxState-1)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	int32 MaxState = 4;
};
