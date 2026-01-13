#include "Gimmick/DoorActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"

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
	OpenRot = ClosedRot + FRotator(0.0f, OpenYaw, 0.0f);

	bIsOpen = bStartOpen;
	Hinge->SetRelativeRotation(bIsOpen ? OpenRot : ClosedRot);
}

void ADoorActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateRotate(DeltaSeconds);
}

// ==================== SFX ====================

void ADoorActor::PlaySFX(USoundBase* Sound)
{
	if (!Sound) return;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		GetSFXLocation(),
		FRotator::ZeroRotator,
		SFX_Volume,
		SFX_Pitch,
		0.0f,
		SFX_Attenuation
	);
}

FVector ADoorActor::GetSFXLocation() const
{
	return DoorMesh ? DoorMesh->GetComponentLocation() : GetActorLocation();
}

// ==================== Interact ====================

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
		return;

	if (bRotating) 
		return;

	if (bIsOpen)
	{
		bIsOpen = false;
		bTargetOpenForSFX = false;

		PlaySFX(SFX_CloseStart);
		StartRotateTo(ClosedRot, /*bTargetOpen*/ false);
		return;
	}

	// 열기
	bIsOpen = true;

	float Sign = 1.0f;
	if (bUseDotForDirection)
	{
		Sign = ComputeOpenSignFromDot(Interactor);
	}

	OpenRot = ClosedRot + FRotator(0.0f, OpenYaw * Sign, 0.0f);

	bTargetOpenForSFX = true;
	PlaySFX(SFX_OpenStart);

	StartRotateTo(OpenRot, /*bTargetOpen*/ true);
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

// ==================== Rotate ====================

void ADoorActor::StartRotateTo(const FRotator& NewTarget, bool bTargetOpen)
{
	UE_LOG(LogTemp, Warning, TEXT("[Door] StartRotateTo OpenYaw=%.1f Closed=%s Open=%s NewTarget=%s"),
	   OpenYaw,
	   *ClosedRot.ToString(),
	   *OpenRot.ToString(),
	   *NewTarget.ToString()
   );
	RotateElapsed = 0.0f;
	bRotating = true;
	bTargetOpenForSFX = bTargetOpen;

	SetActorTickEnabled(true);

	StartQuat = Hinge->GetRelativeRotation().Quaternion();
	TargetQuat = NewTarget.Quaternion();
}

void ADoorActor::UpdateRotate(float DeltaSeconds)
{
	if (!bRotating) { SetActorTickEnabled(false); return; }

	RotateElapsed += DeltaSeconds;

	const float Duration = FMath::Max(RotateTime, 0.01f);
	const float Alpha = FMath::Clamp(RotateElapsed / Duration, 0.f, 1.f);
	const float SmoothAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

	const FQuat NewQuat = FQuat::Slerp(StartQuat, TargetQuat, SmoothAlpha).GetNormalized();

	const FRotator BeforeRel  = Hinge->GetRelativeRotation();
	const FRotator BeforeWorld= Hinge->GetComponentRotation();

	Hinge->SetRelativeRotation(NewQuat.Rotator());

	const FRotator AfterRel   = Hinge->GetRelativeRotation();
	const FRotator AfterWorld = Hinge->GetComponentRotation();

	UE_LOG(LogTemp, Warning,
		TEXT("[DoorRotate] A=%.3f Rel %s -> %s | World %s -> %s | AbsRot=%d | SimPhysMesh=%d | Rep=%d RepMove=%d"),
		Alpha,
		*BeforeRel.ToString(), *AfterRel.ToString(),
		*BeforeWorld.ToString(), *AfterWorld.ToString(),
		Hinge->IsUsingAbsoluteRotation() ? 1 : 0,
		(DoorMesh && DoorMesh->IsSimulatingPhysics()) ? 1 : 0,
		GetIsReplicated() ? 1 : 0,
		IsReplicatingMovement() ? 1 : 0
	);

	if (Alpha >= 1.f)
	{
		Hinge->SetRelativeRotation(TargetQuat.Rotator());
		bRotating = false;
		SetActorTickEnabled(false);
	}
}
// ==================== Tag Gate ====================

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

// ==================== Focus ====================

void ADoorActor::OnFocusBegin_Implementation(AActor* Interactor)
{
	(void)Interactor;
	if (!DoorMesh) return;

	DoorMesh->SetRenderCustomDepth(true);
	DoorMesh->SetCustomDepthStencilValue(1);
}

void ADoorActor::OnFocusEnd_Implementation(AActor* Interactor)
{
	(void)Interactor;
	if (!DoorMesh) return;

	DoorMesh->SetRenderCustomDepth(false);
}
