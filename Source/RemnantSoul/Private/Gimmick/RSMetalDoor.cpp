#include "Gimmick/RSMetalDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

ARSMetalDoor::ARSMetalDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorPanel = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPanel"));
	DoorPanel->SetupAttachment(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPanel);

	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	DoorMesh->SetGenerateOverlapEvents(false);
}

void ARSMetalDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedLoc = DoorPanel->GetRelativeLocation();
	OpenLoc = ClosedLoc + OpenOffset;

	bIsOpen = bStartOpen;
	DoorPanel->SetRelativeLocation(bIsOpen ? OpenLoc : ClosedLoc);

	SetActorTickEnabled(false);
}

void ARSMetalDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateMove(DeltaSeconds);
}

bool ARSMetalDoor::CanInteract_Implementation(AActor* Interactor) const
{
	if (!IsValid(Interactor) || bLocked)
	{
		return false;
	}

	if (!bToggleable && bIsOpen)
	{
		return false;
	}

	return PassesTagGate(Interactor);
}

void ARSMetalDoor::Interact_Implementation(AActor* Interactor)
{
	if (!CanInteract_Implementation(Interactor))
	{
		return;
	}

	// 이미 열려있거나 이동 중이면 무시
	if (bIsOpen || bMoving)
	{
		return;
	}

	bIsOpen = true;

	PlaySFX(SFX_OpenStart);
	StartMoveTo(true); // "위로 올라가는" 타겟으로 이동 시작
}

void ARSMetalDoor::StartMoveTo(bool bOpen)
{
	MoveElapsed = 0.0f;
	bMoving = true;
	SetActorTickEnabled(true);
}

void ARSMetalDoor::UpdateMove(float DeltaSeconds)
{
	if (!bMoving)
	{
		SetActorTickEnabled(false);
		return;
	}

	MoveElapsed += DeltaSeconds;

	const float Duration = FMath::Max(MoveTime, 0.01f);
	const float Alpha = FMath::Clamp(MoveElapsed / Duration, 0.0f, 1.0f);
	const float SmoothAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);

	const FVector Target = bIsOpen ? OpenLoc : ClosedLoc;
	const FVector Start = DoorPanel->GetRelativeLocation();
	const FVector NewLoc = FMath::Lerp(Start, Target, SmoothAlpha);

	DoorPanel->SetRelativeLocation(NewLoc);

	if (Alpha >= 1.0f)
	{
		DoorPanel->SetRelativeLocation(Target);
		bMoving = false;
		SetActorTickEnabled(false);
		PlaySFX(SFX_OpenEnd);

		
	}
}

bool ARSMetalDoor::PassesTagGate(AActor* Interactor) const
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

FVector ARSMetalDoor::GetSFXLocation() const
{
	if (DoorMesh)
	{
		return DoorMesh->GetComponentLocation();
	}
	return GetActorLocation();
}

void ARSMetalDoor::PlaySFX(USoundBase* Sound) const
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