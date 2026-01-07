#include "Gimmick/RSLeverActor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

ARSLeverActor::ARSLeverActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);


	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);


	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(Root);

	
	HandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleMesh"));
	HandleMesh->SetupAttachment(Pivot);


	HandleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandleMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	HandleMesh->SetGenerateOverlapEvents(false);
	
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetGenerateOverlapEvents(false);
}

void ARSLeverActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void ARSLeverActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateLeverAnim(DeltaSeconds);
}

bool ARSLeverActor::CanInteract_Implementation(AActor* Interactor) const
{
	if (!IsValid(Interactor))
	{
		return false;
	}

	if (bHasFired)
	{
		return false; 
	}

	return PassesTagGate(Interactor);
}

void ARSLeverActor::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	bHasFired = true;

	PendingInteractor = Interactor;
	StartLeverAnim();

	if (!bTriggerOnAnimFinished)
	{
		TriggerLinkedTargets(Interactor);
	}
}

bool ARSLeverActor::PassesTagGate(AActor* Interactor) const
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

void ARSLeverActor::StartLeverAnim()
{
	AnimElapsed = 0.0f;
	bAnimating = true;
	PlaySFX(SFX_PullStart);
	const FVector Axis = LocalAxis.GetSafeNormal();
	const float Rad = FMath::DegreesToRadians(AngleDegrees);

	StartQuat = Pivot->GetRelativeRotation().Quaternion();

	const FQuat Delta = FQuat(Axis, Rad);
	TargetQuat = (Delta * StartQuat);
	TargetQuat.Normalize();

	SetActorTickEnabled(true);
}

void ARSLeverActor::UpdateLeverAnim(float DeltaSeconds)
{
	if (!bAnimating)
	{
		SetActorTickEnabled(false);
		return;
	}

	AnimElapsed += DeltaSeconds;

	const float Duration = FMath::Max(PullTime, 0.01f);
	const float Alpha = FMath::Clamp(AnimElapsed / Duration, 0.0f, 1.0f);
	const float SmoothAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);

	FQuat NewQuat = FQuat::Slerp(StartQuat, TargetQuat, SmoothAlpha);
	NewQuat.Normalize();
	Pivot->SetRelativeRotation(NewQuat.Rotator());
	PlaySFX(SFX_PullEnd);

	if (Alpha >= 1.0f)
	{
		Pivot->SetRelativeRotation(TargetQuat.Rotator());
		bAnimating = false;
		SetActorTickEnabled(false);

		if (bTriggerOnAnimFinished && PendingInteractor.IsValid())
		{
			TriggerLinkedTargets(PendingInteractor.Get());
			PendingInteractor = nullptr;
		}
	}
}

void ARSLeverActor::TriggerLinkedTargets(AActor* Interactor)
{
	for (AActor* Target : LinkedTargets)
	{
		if (!IsValid(Target))
		{
			continue;
		}

		if (Target->Implements<UInteractable>())
		{
			if (IInteractable::Execute_CanInteract(Target, Interactor))
			{
				IInteractable::Execute_Interact(Target, Interactor);
			}
		}
	}
}

FVector ARSLeverActor::GetSFXLocation() const
{
	if (HandleMesh)
	{
		return HandleMesh->GetComponentLocation();
	}
	if (Pivot)
	{
		return Pivot->GetComponentLocation();
	}
	return GetActorLocation();
}

void ARSLeverActor::PlaySFX(USoundBase* Sound) const
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

void ARSLeverActor::OnFocusBegin_Implementation(AActor* Interactor)
{
	if (!HandleMesh) return;
	HandleMesh->SetRenderCustomDepth(true);
	HandleMesh->SetCustomDepthStencilValue(1);
	BaseMesh->SetRenderCustomDepth(true);
	BaseMesh->SetCustomDepthStencilValue(1);
}

void ARSLeverActor::OnFocusEnd_Implementation(AActor* Interactor)
{
	if (!HandleMesh) return;
	HandleMesh->SetRenderCustomDepth(false);
	BaseMesh->SetRenderCustomDepth(false);
}
