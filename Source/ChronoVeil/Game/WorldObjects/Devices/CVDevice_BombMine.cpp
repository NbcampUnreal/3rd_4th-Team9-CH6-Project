#include "Game/WorldObjects/Devices/CVDevice_BombMine.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

ACVDevice_BombMine::ACVDevice_BombMine()
{
	if (Collision)
	{
		Collision->SetSphereRadius(TriggerRadius);
		Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	}

	PrimaryActorTick.bCanEverTick = false;
}

void ACVDevice_BombMine::BeginPlay()
{
	Super::BeginPlay();

	// 도화선(점화)VFX를 Device에 붙여 재생
	if (FuseVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			FuseVFX,
			GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}

void ACVDevice_BombMine::OnDeviceOverlap(
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

	// 일단 쿨다운 이펙트 대신 하드코딩으로 한번만 폭발하게 구현. 나중에 쿨다운클래스(Effect클래스)만들어야할듯.
	if (!bArmed && OtherActor && OtherActor != this)
	{
		bArmed = true;

		// 딜레이 후 폭발
		GetWorld()->GetTimerManager().SetTimer(
			ExplosionTimer,
			this,
			&ACVDevice_BombMine::Explode,
			FMath::Max(0.f, ExplosionDelay),
			false
		);
	}
}

void ACVDevice_BombMine::Explode()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ExplosionVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionVFX,
			GetActorLocation(),
			FRotator::ZeroRotator
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		static_cast<float>(ExplosionDamage),
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		this,
		GetInstigatorController(),
		true
	);

	Destroy();
}
