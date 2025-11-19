#include "Game/WorldObjects/Projectiles/CVProjectile_Base.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACVProjectile_Base::ACVProjectile_Base()
{
    PrimaryActorTick.bCanEverTick = false;

    // WorldObject_Base에서 SceneRoot를 만들고 Root로 세팅하지만,
    // 투사체는 Collision을 Root로 쓰는 쪽이 자연스러워서 여기서 덮어쓴다.
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(50.f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // Hit 이벤트용 Blocking 프로파일
    SetRootComponent(Collision);

    Collision->OnComponentHit.AddDynamic(this, &ACVProjectile_Base::OnProjectileHit);

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
    // 파생에서 오버라이드해서 데미지/Zone 스폰/FX 등을 구현
}

void ACVProjectile_Base::HandleLifeTimeExpired()
{
    if (!HasAuthority())
    {
        return;
    }

    Destroy();
}
