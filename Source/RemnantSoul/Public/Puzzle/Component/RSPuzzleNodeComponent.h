#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSPuzzleNodeComponent.generated.h"

UCLASS(ClassGroup=(Puzzle), meta=(BlueprintSpawnableComponent))
class REMNANTSOUL_API URSPuzzleNodeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	FGameplayTag PuzzleTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	FGameplayTag NodeTag;

	// 상태 범위: 0..(MaxState-1)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle")
	int32 MaxState = 4;

	// 인터랙트 시 증가(+1 mod MaxState)
	UFUNCTION(BlueprintCallable)
	void AdvanceStateAndSubmit(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	int32 GetLocalState() const { return LocalState; }

private:
	UPROPERTY(VisibleAnywhere)
	int32 LocalState = 0;

	void Submit(AActor* InstigatorActor);
};
