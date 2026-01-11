#include "WorldObjects/Zones/RSZone_MeteorImpact.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"

ARSZone_MeteorImpact::ARSZone_MeteorImpact()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(150.f);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(Collision);

	ZoneShape = Collision;
	TickInterval = 0.f;

	MeteorVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MeteorVFX"));
	MeteorVFX->SetupAttachment(RootComponent);
	MeteorVFX->SetAutoActivate(false);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ARSZone_MeteorImpact::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (FallingVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FallingVFX,
				GetActorLocation() + FVector(0.f, 0.f, 800.f),
				FRotator::ZeroRotator,
				FVector(1.0f),
				true,
				true
			);
		}

		GetWorld()->GetTimerManager().SetTimer(
			ImpactTimer,
			this,
			&ThisClass::OnImpact,
			ImpactDelay,
			false
		);
	}
}

void ARSZone_MeteorImpact::OnImpact()
{
	if (!HasAuthority())
	{
		return;
	}

	TArray<AActor*> Overlaps;
	Collision->GetOverlappingActors(Overlaps, ACharacter::StaticClass());

	for (AActor* Target : Overlaps)
	{
		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}

	Multicast_PlayImpactVFX();

	Destroy();
}

void ARSZone_MeteorImpact::Multicast_PlayImpactVFX_Implementation()
{
	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactVFX,
			GetActorLocation() + FVector(0.f, 0.f, -70.f),
			FRotator::ZeroRotator
		);
	}
}
