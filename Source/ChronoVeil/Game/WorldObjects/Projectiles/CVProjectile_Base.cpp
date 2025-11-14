#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACVProjectile_Base::ACVProjectile_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	// 콜리전
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(15.f);
	Collision->SetCollisionProfileName(TEXT("Projectile")); // 프로젝트에 맞게 프로파일 설정
	SetRootComponent(Collision);

	// 히트 이벤트
	Collision->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHit);

	// 이동 컴포넌트
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 3000.f;
	Movement->MaxSpeed = 3000.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACVProjectile_Base::BeginPlay()
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

void ACVProjectile_Base::OnProjectileHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	// 서버만 충돌 처리
	if (!HasAuthority())
	{
		return;
	}

	// 자기 자신이나 소유자와의 히트는 무시할지 여부는 여기서 필터링 가능
	if (OtherActor == this)
	{
		return;
	}

	HandleImpact(Hit);
	Destroy();
}

void ACVProjectile_Base::HandleImpact(const FHitResult& Hit)
{
	// 기본 구현: 아무것도 안 함
	// 파생 클래스 (클러스터 수류탄, 메테오 등)에서 오버라이드해서
	// - 데미지
	// - Zone 스폰
	// - FX 재생
	// 등을 수행
}

void ACVProjectile_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}
