#include "WorldObjects/Projectiles/RSProjectile_ClusterGrenade.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/ProjectileMovementComponent.h"

ARSProjectile_ClusterGrenade::ARSProjectile_ClusterGrenade()
{
	PrimaryActorTick.bCanEverTick = false;

	if (Collision)
	{
		Collision->SetSimulatePhysics(true);
		Collision->SetEnableGravity(true);
		Collision->SetNotifyRigidBodyCollision(true);
		Collision->SetCollisionProfileName(TEXT("PhysicsActor"));

		Collision->SetLinearDamping(0.1f);
		Collision->SetAngularDamping(0.1f);
		Collision->SetMassOverrideInKg(NAME_None, 2.5f, true);
	}

	if (Movement)
	{
		Movement->bAutoActivate = false;
		Movement->Deactivate();
	}

	ExplosionRadiusComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRadius"));
	ExplosionRadiusComp->SetupAttachment(Collision);
	ExplosionRadiusComp->InitSphereRadius(ExplosionRadius);
	ExplosionRadiusComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionRadiusComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionRadiusComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ARSProjectile_ClusterGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (ExplosionRadiusComp)
	{
		ExplosionRadiusComp->SetSphereRadius(ExplosionRadius);
	}

	if (Collision)
	{
		UPhysicalMaterial* PhysMat = NewObject<UPhysicalMaterial>(this);
		PhysMat->Friction = 0.4f;
		PhysMat->Restitution = 0.8f;

		Collision->SetPhysMaterialOverride(PhysMat);
	}

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_Explode,
			this,
			&ThisClass::Explode,
			FuseTime,
			false
		);
	}
}

void ARSProjectile_ClusterGrenade::OnProjectileHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	/* 여기서는 “튕김”만, Destroy는 FuseTime에서 */
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		FVector Velocity = RootPrim->GetPhysicsLinearVelocity();

		Velocity *= 0.8f;

		if (FMath::Abs(Hit.Normal.Z) > 0.6f)
		{
			Velocity.Z *= -0.6f;
		}

		RootPrim->SetPhysicsLinearVelocity(Velocity);

		if (BounceSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BounceSound, GetActorLocation());
		}
	}
}

void ARSProjectile_ClusterGrenade::Explode()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}

	Multicast_PlayExplosionFX();

	if (ExplosionRadiusComp)
	{
		TArray<AActor*> OverlappingActors;
		ExplosionRadiusComp->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

		for (AActor* Target : OverlappingActors)
		{
			UGameplayStatics::ApplyDamage(
				Target,
				ExplosionDamage,
				GetInstigatorController(),
				this,
				nullptr
			);
		}
	}

	Destroy();
}

void ARSProjectile_ClusterGrenade::Multicast_PlayExplosionFX_Implementation()
{
	if (ExplosionVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionVFX,
			GetActorLocation(),
			FRotator::ZeroRotator
		);
	}
}
