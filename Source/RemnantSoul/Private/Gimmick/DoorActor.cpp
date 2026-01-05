#include "Gimmick/DoorActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);

	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	DoorMesh->SetGenerateOverlapEvents(false);
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false); 

	ClosedRot = Hinge->GetRelativeRotation();

	// 시작 열림이면 기본적으로 +OpenYaw 방향(원하면 BP에서 bInvertChosenDirection로 뒤집기)
	OpenRot = ClosedRot + FRotator(0.0f, OpenYaw, 0.0f);

	bIsOpen = bStartOpen;
	Hinge->SetRelativeRotation(bIsOpen ? OpenRot : ClosedRot);
}

void ADoorActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateRotate(DeltaSeconds);
}

bool ADoorActor::CanInteract_Implementation(AActor* Interactor) const
{
	if (!IsValid(Interactor) || bLocked)
	{
		return false;
	}

	return PassesTagGate(Interactor);
}

void ADoorActor::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	if (bIsOpen)
	{
		bIsOpen = false;
		StartRotateTo(ClosedRot);
		return;
	}

	// 닫힘 -> 열림: 내적으로 방향 결정
	bIsOpen = true;

	float Sign = 1.0f;
	if (bUseDotForDirection)
	{
		Sign = ComputeOpenSignFromDot(Interactor);
	}

	OpenRot = ClosedRot + FRotator(0.0f, OpenYaw * Sign, 0.0f);
	StartRotateTo(OpenRot);
}

float ADoorActor::ComputeOpenSignFromDot(AActor* Interactor) const
{
	const FVector ToInteractor = (Interactor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const float Dot = FVector::DotProduct(GetActorForwardVector(), ToInteractor);

	
	float Sign = 1.0f;
	if (bOpenAwayFromInteractor)
	{
		Sign = (Dot > 0.0f) ? -1.0f : 1.0f;
	}
	else
	{
		Sign = (Dot > 0.0f) ? 1.0f : -1.0f;
	}

	if (bInvertChosenDirection)
	{
		Sign *= -1.0f;
	}

	return Sign;
}

void ADoorActor::StartRotateTo(const FRotator& NewTarget)
{
	RotateElapsed = 0.0f;
	bRotating = true;
	SetActorTickEnabled(true);

	StartQuat = Hinge->GetRelativeRotation().Quaternion();
	TargetQuat = NewTarget.Quaternion();
}

void ADoorActor::UpdateRotate(float DeltaSeconds)
{
	if (!bRotating)
	{
		SetActorTickEnabled(false);
		return;
	}

	RotateElapsed += DeltaSeconds;

	const float Duration = FMath::Max(RotateTime, 0.01f);
	const float Alpha = FMath::Clamp(RotateElapsed / Duration, 0.0f, 1.0f);

	// SmoothStep(가감속)
	const float SmoothAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);

	FQuat NewQuat = FQuat::Slerp(StartQuat, TargetQuat, SmoothAlpha);
	NewQuat.Normalize();

	Hinge->SetRelativeRotation(NewQuat.Rotator());

	if (Alpha >= 1.0f)
	{
		Hinge->SetRelativeRotation(TargetQuat.Rotator());
		bRotating = false;
		SetActorTickEnabled(false);
	}
	
}

bool ADoorActor::PassesTagGate(AActor* Interactor) const
{
	if (InteractorRequiredTags.IsEmpty() && InteractorBlockedTags.IsEmpty())
	{
		return true; 
	}

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Interactor);
	if (!ASI)
	{
		return true; 
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return true;
	}

	if (!InteractorBlockedTags.IsEmpty() && ASC->HasAnyMatchingGameplayTags(InteractorBlockedTags))
	{
		return false;
	}

	if (!InteractorRequiredTags.IsEmpty() && !ASC->HasAllMatchingGameplayTags(InteractorRequiredTags))
	{
		return false;
	}

	return true;
}
