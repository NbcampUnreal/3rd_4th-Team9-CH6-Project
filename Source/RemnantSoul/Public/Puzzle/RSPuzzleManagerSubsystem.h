#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "RSPuzzleDefinition.h"
#include "RSPuzzleManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRSPuzzleStateChanged, FGameplayTag, PuzzleTag, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSPuzzleSolved, FGameplayTag, PuzzleTag);

USTRUCT()
struct FRSPuzzleRuntime
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<const URSPuzzleDefinition> Definition = nullptr;

	UPROPERTY()
	TMap<FGameplayTag, int32> CurrentStates;

	UPROPERTY()
	bool bSolved = false;
};

UCLASS()
class REMNANTSOUL_API URSPuzzleManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FRSPuzzleStateChanged OnPuzzleStateChanged;

	UPROPERTY(BlueprintAssignable)
	FRSPuzzleSolved OnPuzzleSolved;

	UFUNCTION(BlueprintCallable)
	bool StartPuzzle(const URSPuzzleDefinition* Definition, AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	void EndPuzzle(FGameplayTag PuzzleTag);

	UFUNCTION(BlueprintCallable)
	bool IsPuzzleActive(FGameplayTag PuzzleTag) const;

	UFUNCTION(BlueprintCallable)
	bool SubmitNode(FGameplayTag PuzzleTag, FGameplayTag NodeTag, int32 NewState);

	UFUNCTION(BlueprintCallable)
	int32 GetNodeState(FGameplayTag PuzzleTag, FGameplayTag NodeTag, int32 DefaultValue = 0) const;

private:
	UPROPERTY()
	TMap<FGameplayTag, FRSPuzzleRuntime> ActivePuzzles;

	bool EvaluateSolved(FRSPuzzleRuntime& Runtime) const;

	// (선택) 퍼즐 시작/종료 시 플레이어에 GE 적용 훅
	void ApplyPuzzleMode(AActor* InstigatorActor, bool bEnable);
};
