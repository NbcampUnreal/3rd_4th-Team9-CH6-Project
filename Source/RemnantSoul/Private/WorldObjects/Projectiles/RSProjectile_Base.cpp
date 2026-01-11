#include "WorldObjects/Projectiles/RSProjectile_Base.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARSProjectile_Base::ARSProjectile_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(50.f);
	Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(Collision);

	Collision->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 3000.f;
	Movement->MaxSpeed = 3000.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->SetUpdatedComponent(Collision);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ARSProjectile_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && LifeTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			LifeHandle,
			this,
			&ThisClass::HandleLifeTimeExpired,
			LifeTime,
			false
		);
	}
}

void ARSProjectile_Base::OnProjectileHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (OtherActor == this)
	{
		return;
	}

	HandleImpact(Hit);
	Destroy();
}

void ARSProjectile_Base::HandleImpact(const FHitResult& Hit)
{
}

void ARSProjectile_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}
