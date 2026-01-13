#include "WorldObjects/Projectiles/RSProjectile_Armageddon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Controller.h"

FVector ARSProjectile_Armageddon::ToVector(const FVector_NetQuantize& V)
{
	// UE에서 FVector_NetQuantize가 FVector로 암시 변환이 항상 보장되는 게 아니라서
	// 아예 명시적으로 안전 변환을 고정한다.
	return FVector(V.X, V.Y, V.Z);
}

ARSProjectile_Armageddon::ARSProjectile_Armageddon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Base(ARSProjectile_Base)에서 Movement를 CreateDefaultSubobject로 만들기 때문에
	// 여기서는 값만 튜닝한다.
	if (Movement)
	{
		Movement->InitialSpeed = 2200.f;
		Movement->MaxSpeed = 2200.f;
		Movement->bRotationFollowsVelocity = true;
		Movement->bShouldBounce = false;
	}

	TrailVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFXComponent->SetupAttachment(RootComponent);
	TrailVFXComponent->SetAutoActivate(false);

	// 기본 수명(필요하면 BP/DA에서 오버라이드)
	LifeTime = 5.f;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ARSProjectile_Armageddon::BeginPlay()
{
	Super::BeginPlay();

	// Trail은 클라에서도 보여야 하므로 BeginPlay에서 켜도 됨(서버만 제한할 필요 없음)
	if (TrailVFXComponent && TrailVFX)
	{
		TrailVFXComponent->SetAsset(TrailVFX);
		TrailVFXComponent->Activate(true);
	}
}

void ARSProjectile_Armageddon::HandleImpact(const FHitResult& Hit)
{
	// Base OnProjectileHit가 서버에서만 HandleImpact를 부르도록 되어 있어도,
	// 여기서 한 번 더 가드치면 안전하다.
	if (!HasAuthority())
	{
		return;
	}

	// 1) 데미지 (서버)
	AActor* HitActor = Hit.GetActor();
	if (HitActor && HitActor != this)
	{
		UGameplayStatics::ApplyDamage(
			HitActor,
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}

	// 2) Impact 위치/회전 계산 (삼항 연산자 대신 if로 고정해서 컴파일 이슈 방지)
	FVector ImpactLoc = Hit.ImpactPoint;
	if (ImpactLoc.IsNearlyZero())
	{
		ImpactLoc = GetActorLocation();
	}

	const FRotator ImpactRot = Hit.ImpactNormal.Rotation();

	// 3) FX/Sound는 멀티캐스트로 전체 반영
	Multicast_PlayImpactFX(FVector_NetQuantize(ImpactLoc), ImpactRot);
}

void ARSProjectile_Armageddon::Multicast_PlayImpactFX_Implementation(const FVector_NetQuantize& Loc, const FRotator& Rot)
{
	const FVector WorldLoc = ToVector(Loc);

	if (ImpactVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactVFX,
			WorldLoc,
			Rot
		);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ImpactSound,
			WorldLoc
		);
	}
}
