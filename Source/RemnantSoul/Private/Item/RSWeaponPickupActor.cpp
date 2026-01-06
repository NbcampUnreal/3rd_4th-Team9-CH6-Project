#include "Item/RSWeaponPickupActor.h"
#include "Item/RSItemTemplate.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/RSCharacter.h"

ARSWeaponPickupActor::ARSWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(Root);
	Sphere->InitSphereRadius(80.f);

	// Overlap 이벤트가 발생하려면 이 2개가 핵심
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);

	VisualMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(Root);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARSWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	}
}

void ARSWeaponPickupActor::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bConsumed)
	{
		return;
	}
	bConsumed = true;

	UE_LOG(LogTemp, Warning, TEXT("[Pickup] Overlap OtherActor=%s"), *GetNameSafe(OtherActor));

	ARSCharacter* Char = Cast<ARSCharacter>(OtherActor);
	if (!Char)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Pickup] Overlap but OtherActor is not RSCharacter"));
		return;
	}

	const URSItemTemplate* Template = PickupItemTemplate.Get();

	UE_LOG(LogTemp, Warning, TEXT("[Pickup] Template=%s Count=%d"),
		*GetNameSafe(Template),
		PickupCount);

	const bool bOK = Char->PickupFromActor(this, /*bAutoEquip=*/true);

	UE_LOG(LogTemp, Warning, TEXT("[Pickup] PickupFromActor result=%d"), bOK);

	if (!bOK)
	{
		return;
	}

	if (!bOK)
	{
		bConsumed = false; // 실패하면 재시도 가능
		return;
	}

	Destroy();
}
