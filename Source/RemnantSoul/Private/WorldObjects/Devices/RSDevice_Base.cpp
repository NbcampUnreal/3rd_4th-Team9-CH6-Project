#include "WorldObjects/Devices/RSDevice_Base.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARSDevice_Base::ARSDevice_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(50.f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDeviceOverlap);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ARSDevice_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (LifeTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LifeTimerHandle,
				this,
				&ThisClass::HandleLifeTimeExpired,
				LifeTime,
				false
			);
		}
	}
}

void ARSDevice_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}

void ARSDevice_Base::OnDamaged(float Damage, AActor* DamageCauser)
{
}

void ARSDevice_Base::OnDeviceOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}
}
