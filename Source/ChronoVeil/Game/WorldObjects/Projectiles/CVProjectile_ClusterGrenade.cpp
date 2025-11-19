#include "Game/WorldObjects/Projectiles/CVProjectile_ClusterGrenade.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACVProjectile_ClusterGrenade::ACVProjectile_ClusterGrenade()
{
    // Base에서 만든 Collision을 물리 그레네이드로 사용햠./
    if (Collision)
    {
        Collision->SetSimulatePhysics(true);
        Collision->SetEnableGravity(true);
        Collision->SetNotifyRigidBodyCollision(true);
        Collision->SetCollisionProfileName(TEXT("PhysicsActor"));

        Collision->SetLinearDamping(0.1f);   // 공기 저항
        Collision->SetAngularDamping(0.1f);  // 회전 감쇠
        Collision->SetMassOverrideInKg(NAME_None, 2.5f, true);
    }

    // ProjectileMovement는 이 투사체에선 사용하지 않음
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
}

void ACVProjectile_ClusterGrenade::BeginPlay()
{
    Super::BeginPlay();

    // 물리 머티리얼 생성 (BeginPlay 시점)
    if (Collision)
    {
        UPhysicalMaterial* PhysMat = NewObject<UPhysicalMaterial>(this);
        PhysMat->Friction = 0.4f;
        PhysMat->Restitution = 0.8f;  // 튕김 정도

        Collision->SetPhysMaterialOverride(PhysMat);
    }

    // 서버만 폭발 타이머
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(
            TimerHandle_Explode,
            this,
            &ACVProjectile_ClusterGrenade::Explode,
            FuseTime,
            false
        );
    }
}

void ACVProjectile_ClusterGrenade::OnProjectileHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    // 여기서는 "튕기는" 동작만 처리하고, Base처럼 Destroy()는 하지 않음.
    if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetRootComponent()))
    {
        FVector Velocity = RootPrim->GetPhysicsLinearVelocity();

        // 속도 감쇠 (무한 튕김 방지)
        Velocity *= 0.8f;

        // 바닥과 부딪힐 때 Z 방향 반사
        if (FMath::Abs(Hit.Normal.Z) > 0.6f) // 거의 바닥면
        {
            Velocity.Z *= -0.6f;
        }

        RootPrim->SetPhysicsLinearVelocity(Velocity);

        if (BounceSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), BounceSound, GetActorLocation());
        }
    }

    // 폭발은 FuseTime 타이머에서 처리할것임.
}

void ACVProjectile_ClusterGrenade::Explode()
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

    // 폭발 반경 내 캐릭터에게 피해 적용
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

    Destroy();
}

void ACVProjectile_ClusterGrenade::Multicast_PlayExplosionFX_Implementation()
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
