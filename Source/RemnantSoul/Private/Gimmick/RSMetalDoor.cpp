#include "Gimmick/RSMetalDoor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"

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

	// 라인트레이스(Visibility) 히트용
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

// ==================== Lever-only API ====================

void ARSMetalDoor::OpenByLever()
{
	if (bLocked || bMoving)
		return;

	if (bIsOpen)
		return;

	bIsOpen = true;

	PlaySFX(SFX_OpenStart);
	StartMoveTo(true);
}

void ARSMetalDoor::CloseByLever()
{
	if (bLocked || bMoving)
		return;

	if (!bToggleable) // 토글 불가면 닫기 금지
		return;

	if (!bIsOpen)
		return;

	bIsOpen = false;

	PlaySFX(SFX_CloseStart);
	StartMoveTo(false);
}

void ARSMetalDoor::ToggleByLever()
{
	if (bIsOpen)
	{
		CloseByLever();
	}
	else
	{
		OpenByLever();
	}
}

// ==================== IInteractable (Direct interact disabled) ====================

bool ARSMetalDoor::CanInteract_Implementation(AActor* /*Interactor*/) const
{
	// ✅ 플레이어(Ability_Interact)로는 절대 열 수 없게 고정
	return false;
}

void ARSMetalDoor::Interact_Implementation(AActor* /*Interactor*/)
{
	// 의도적으로 no-op
}

// ==================== Move ====================

void ARSMetalDoor::StartMoveTo(bool /*bOpen*/)
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

	// ✅ Start는 "한 번"만 고정해둬야 보간이 안정적
	const FVector Start = bIsOpen ? ClosedLoc : OpenLoc;
	const FVector Target = bIsOpen ? OpenLoc : ClosedLoc;
	const FVector NewLoc = FMath::Lerp(Start, Target, SmoothAlpha);

	DoorPanel->SetRelativeLocation(NewLoc);

	if (Alpha >= 1.0f)
	{
		DoorPanel->SetRelativeLocation(Target);
		bMoving = false;
		SetActorTickEnabled(false);

		PlaySFX(bIsOpen ? SFX_OpenEnd : SFX_CloseEnd);
	}
}

// ==================== Tag Gate (optional) ====================

bool ARSMetalDoor::PassesTagGate(AActor* Interactor) const
{
	if (InteractorRequiredTags.IsEmpty() && InteractorBlockedTags.IsEmpty())
		return true;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Interactor);
	if (!ASI)
		return true;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
		return true;

	if (!InteractorBlockedTags.IsEmpty() && ASC->HasAnyMatchingGameplayTags(InteractorBlockedTags))
		return false;

	if (!InteractorRequiredTags.IsEmpty() && !ASC->HasAllMatchingGameplayTags(InteractorRequiredTags))
		return false;

	return true;
}

// ==================== SFX ====================

FVector ARSMetalDoor::GetSFXLocation() const
{
	return DoorMesh ? DoorMesh->GetComponentLocation() : GetActorLocation();
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
