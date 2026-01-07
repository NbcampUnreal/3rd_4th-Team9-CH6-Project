#include "Puzzle/RSPuzzleManagerSubsystem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RSGameplayTags.h"

bool URSPuzzleManagerSubsystem::StartPuzzle(const URSPuzzleDefinition* Definition, AActor* InstigatorActor)
{
	if (!Definition || !Definition->PuzzleTag.IsValid())
	{
		return false;
	}

	const FGameplayTag PuzzleTag = Definition->PuzzleTag;

	FRSPuzzleRuntime& Runtime = ActivePuzzles.FindOrAdd(PuzzleTag);
	Runtime.Definition = Definition;
	Runtime.bSolved = false;
	Runtime.CurrentStates.Reset();

	// 초기 상태: 목표 노드들을 0으로 초기화 (원하면 DataAsset에 초기값 필드 추가 가능)
	for (const FRSPuzzleNodeTarget& T : Definition->Targets)
	{
		if (T.NodeTag.IsValid())
		{
			Runtime.CurrentStates.Add(T.NodeTag, 0);
		}
	}

	ApplyPuzzleMode(InstigatorActor, true);
	OnPuzzleStateChanged.Broadcast(PuzzleTag, true);

	return true;
}

void URSPuzzleManagerSubsystem::EndPuzzle(FGameplayTag PuzzleTag)
{
	if (!PuzzleTag.IsValid())
	{
		return;
	}

	ActivePuzzles.Remove(PuzzleTag);
	OnPuzzleStateChanged.Broadcast(PuzzleTag, false);
}

bool URSPuzzleManagerSubsystem::IsPuzzleActive(FGameplayTag PuzzleTag) const
{
	return ActivePuzzles.Contains(PuzzleTag);
}

int32 URSPuzzleManagerSubsystem::GetNodeState(FGameplayTag PuzzleTag, FGameplayTag NodeTag, int32 DefaultValue) const
{
	const FRSPuzzleRuntime* Runtime = ActivePuzzles.Find(PuzzleTag);
	if (!Runtime)
	{
		return DefaultValue;
	}

	if (const int32* State = Runtime->CurrentStates.Find(NodeTag))
	{
		return *State;
	}

	return DefaultValue;
}

bool URSPuzzleManagerSubsystem::SubmitNode(FGameplayTag PuzzleTag, FGameplayTag NodeTag, int32 NewState)
{
	FRSPuzzleRuntime* Runtime = ActivePuzzles.Find(PuzzleTag);
	if (!Runtime || !Runtime->Definition || Runtime->bSolved)
	{
		return false;
	}

	Runtime->CurrentStates.FindOrAdd(NodeTag) = NewState;

	if (EvaluateSolved(*Runtime))
	{
		Runtime->bSolved = true;

		// 성공 처리: 여기서 문 열기/레버 트리거 등 브로드캐스트로 분리
		OnPuzzleSolved.Broadcast(PuzzleTag);

		// 퍼즐 모드 해제(원하면 자동 종료)
		ApplyPuzzleMode(nullptr, false);
		EndPuzzle(PuzzleTag);

		return true;
	}

	return false;
}

bool URSPuzzleManagerSubsystem::EvaluateSolved(FRSPuzzleRuntime& Runtime) const
{
	const URSPuzzleDefinition* Def = Runtime.Definition;
	if (!Def)
	{
		return false;
	}

	switch (Def->Rule)
	{
	case ERSPuzzleRule::MatchAllStates:
		{
			for (const FRSPuzzleNodeTarget& T : Def->Targets)
			{
				if (!T.NodeTag.IsValid())
				{
					continue;
				}

				const int32 Current = Runtime.CurrentStates.FindRef(T.NodeTag);
				if (Current != T.TargetState)
				{
					return false;
				}
			}
			return true;
		}
	default:
		break;
	}

	return false;
}

void URSPuzzleManagerSubsystem::ApplyPuzzleMode(AActor* InstigatorActor, bool bEnable)
{
	// 지금은 싱글/간단 운영: 태그만 걸어도 되고, GE로 “능력 차단”을 걸어도 됨.
	// 네가 GA/GE로 통제하려면 여기에 GE_PuzzleMode 적용 코드를 붙이면 된다.

	(void)InstigatorActor;
	(void)bEnable;

	// 예시(나중에):
	// UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor);
	// if (ASC) { Apply GE ... }
}
