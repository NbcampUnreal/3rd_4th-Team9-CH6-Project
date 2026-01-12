#include "Puzzle/RSRuneNode.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ARSRuneNode::ARSRuneNode()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Root);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Visual->SetRenderCustomDepth(false);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);
	Trigger->SetBoxExtent(FVector(30.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ARSRuneNode::BeginPlay()
{
	Super::BeginPlay();
	ApplyVisualRotation();
}

bool ARSRuneNode::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor);
}

void ARSRuneNode::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor) || !IsValid(Visual))
	{
		return;
	}

	const int32 SafeMax = FMath::Max(1, MaxState);
	LocalState = (LocalState + 1) % SafeMax;
	ApplyVisualRotation();
}

void ARSRuneNode::OnFocusBegin_Implementation(AActor* Interactor)
{
	if (bHighlightOnFocus)
	{
		SetHighlight(true);
	}
}

void ARSRuneNode::OnFocusEnd_Implementation(AActor* Interactor)
{
	if (bHighlightOnFocus)
	{
		SetHighlight(false);
	}
}

void ARSRuneNode::ApplyVisualRotation() const
{
	if (!Visual)
	{
		return;
	}

	const int32 SafeMax = FMath::Max(1, MaxState);
	const float StepDeg = 360.f / static_cast<float>(SafeMax);
	const float AngleDeg = StepDeg * static_cast<float>(LocalState);

	const FVector Axis = RotationAxis.GetSafeNormal();
	const FQuat Delta(Axis.IsNearlyZero() ? FVector::UpVector : Axis, FMath::DegreesToRadians(AngleDeg));
	const FQuat Base = BaseRotation.Quaternion();

	// Apply Base first, then Delta (Q2 * Q1 applies Q1 then Q2)
	Visual->SetRelativeRotation((Delta * Base).Rotator());
}

void ARSRuneNode::SetHighlight(bool bEnable) const
{
	if (!Visual)
	{
		return;
	}

	Visual->SetRenderCustomDepth(bEnable);
	if (bEnable)
	{
		Visual->SetCustomDepthStencilValue(static_cast<int32>(FMath::Clamp(HighlightStencilValue, 0, 255)));
	}
}