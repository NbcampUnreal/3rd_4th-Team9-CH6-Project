#include "Puzzle/Component/RSPuzzleNodeComponent.h"
#include "Puzzle/RSPuzzleManagerSubsystem.h"

void URSPuzzleNodeComponent::AdvanceStateAndSubmit(AActor* InstigatorActor)
{
	if (!PuzzleTag.IsValid() || !NodeTag.IsValid() || MaxState <= 0)
	{
		return;
	}

	LocalState = (LocalState + 1) % MaxState;
	Submit(InstigatorActor);
}

void URSPuzzleNodeComponent::Submit(AActor* InstigatorActor)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	URSPuzzleManagerSubsystem* Manager = World->GetSubsystem<URSPuzzleManagerSubsystem>();
	if (!Manager)
	{
		return;
	}

	Manager->SubmitNode(PuzzleTag, NodeTag, LocalState);
}
