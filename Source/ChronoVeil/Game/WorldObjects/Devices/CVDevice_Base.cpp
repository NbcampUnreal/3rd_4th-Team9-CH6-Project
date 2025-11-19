#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"


ACVDevice_Base::ACVDevice_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(50.f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

	Collision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ACVDevice_Base::OnDeviceOverlap
	);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACVDevice_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 수명 타이머
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

		// 필요하면 Owner 죽을 때 Destroy 연동은 나중에 ASC / Health 쪽에서 이벤트 받아서 처리
	}
}

void ACVDevice_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}

void ACVDevice_Base::OnDamaged(float Damage, AActor* DamageCauser)
{

}

void ACVDevice_Base::OnDeviceOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
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
