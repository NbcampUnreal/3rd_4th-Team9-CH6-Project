// RSRuneNode.cpp

#include "Puzzle/RSRuneNode.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"

#include "Puzzle/Component/RSPuzzleNodeComponent.h"
#include "Puzzle/RSPuzzleDefinition.h"
#include "Puzzle/RSPuzzleManagerSubsystem.h"

namespace RSRuneNode_Internal
{
	static FGameplayTag TryTag(const TCHAR* Name)
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(FName(Name), /*ErrorIfNotFound*/ false);
	}
}

ARSRuneNode::ARSRuneNode()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Root);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(30.f, 30.f, 30.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PuzzleNode = CreateDefaultSubobject<URSPuzzleNodeComponent>(TEXT("PuzzleNode"));

	// ✅ 전부 하드코딩
	PuzzleNode->MaxState = MaxState_Hardcoded;
	PuzzleNode->LocalState = 0;
}

void ARSRuneNode::BeginPlay()
{
	Super::BeginPlay();

	ApplyHardcodedTagsFromName();
	UpdateVisualByLocalState();
}

bool ARSRuneNode::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor);
}

void ARSRuneNode::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor) || !IsValid(PuzzleNode))
	{
		return;
	}

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

	if (!PuzzleNode->PuzzleTag.IsValid() || !PuzzleNode->NodeTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RSRuneNode] Invalid PuzzleTag/NodeTag. Check GameplayTags exist."));
		return;
	}

	// 퍼즐이 아직 안 켜져있으면, 첫 입력에서 시작
	if (!Manager->IsPuzzleActive(PuzzleNode->PuzzleTag))
	{
		const URSPuzzleDefinition* Def = GetOrCreateHardcodedDefinition();
		if (!Def)
		{
			return;
		}

		Manager->StartPuzzle(Def, Interactor);
	}

	// 노드 상태 증가 + Submit
	PuzzleNode->AdvanceStateAndSubmit(Interactor);

	// 간단 시각 피드백(회전)
	UpdateVisualByLocalState();
}

URSItemData* ARSRuneNode::GetItemData_Implementation() const
{
	return nullptr;
}

void ARSRuneNode::ApplyHardcodedTagsFromName()
{
	using namespace RSRuneNode_Internal;

	// ✅ 퍼즐 태그 하드코딩
	PuzzleNode->PuzzleTag = TryTag(TEXT("Puzzle.Runes.PillarA"));

	// ✅ 액터 이름으로 Top/Mid/Bot 자동 매칭(하드코딩 규칙)
	const FString N = GetName();

	if (N.Contains(TEXT("Bot"), ESearchCase::IgnoreCase) || N.Contains(TEXT("Bottom"), ESearchCase::IgnoreCase))
	{
		PuzzleNode->NodeTag = TryTag(TEXT("Puzzle.Runes.PillarA.Bot"));
	}
	else if (N.Contains(TEXT("Mid"), ESearchCase::IgnoreCase) || N.Contains(TEXT("Middle"), ESearchCase::IgnoreCase))
	{
		PuzzleNode->NodeTag = TryTag(TEXT("Puzzle.Runes.PillarA.Mid"));
	}
	else
	{
		// 기본값 Top
		PuzzleNode->NodeTag = TryTag(TEXT("Puzzle.Runes.PillarA.Top"));
	}

	if (!PuzzleNode->PuzzleTag.IsValid() || !PuzzleNode->NodeTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RSRuneNode] GameplayTags not found. Add tags in Project Settings > GameplayTags."));
	}
}

const URSPuzzleDefinition* ARSRuneNode::GetOrCreateHardcodedDefinition() const
{
	using namespace RSRuneNode_Internal;

	if (CachedDefinition)
	{
		return CachedDefinition;
	}

	const FGameplayTag PuzzleTag = TryTag(TEXT("Puzzle.Runes.PillarA"));
	const FGameplayTag TopTag   = TryTag(TEXT("Puzzle.Runes.PillarA.Top"));
	const FGameplayTag MidTag   = TryTag(TEXT("Puzzle.Runes.PillarA.Mid"));
	const FGameplayTag BotTag   = TryTag(TEXT("Puzzle.Runes.PillarA.Bot"));

	if (!PuzzleTag.IsValid() || !TopTag.IsValid() || !MidTag.IsValid() || !BotTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RSRuneNode] Cannot build definition: required GameplayTags missing."));
		return nullptr;
	}

	URSPuzzleDefinition* Def = NewObject<URSPuzzleDefinition>(GetTransientPackage(), NAME_None, RF_Transient);
	Def->PuzzleTag = PuzzleTag;
	Def->Rule = ERSPuzzleRule::MatchAllStates;
	Def->Targets.Reset();

	{
		FRSPuzzleNodeTarget T;
		T.NodeTag = TopTag;
		T.TargetState = Target_Top;
		Def->Targets.Add(T);
	}
	{
		FRSPuzzleNodeTarget T;
		T.NodeTag = MidTag;
		T.TargetState = Target_Mid;
		Def->Targets.Add(T);
	}
	{
		FRSPuzzleNodeTarget T;
		T.NodeTag = BotTag;
		T.TargetState = Target_Bot;
		Def->Targets.Add(T);
	}

	CachedDefinition = Def;
	return CachedDefinition;
}

void ARSRuneNode::UpdateVisualByLocalState() const
{
	if (!Visual || !PuzzleNode)
	{
		return;
	}

	const float StepDeg = 360.f / float(FMath::Max(1, PuzzleNode->MaxState));
	const float Yaw = StepDeg * float(PuzzleNode->LocalState);

	Visual->SetRelativeRotation(FRotator(0.f, Yaw, 0.f));
}
