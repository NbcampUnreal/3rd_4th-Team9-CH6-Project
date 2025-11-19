#include "Game/WorldObjects/Zones/CVZone_MeteorImpact.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"



ACVZone_MeteorImpact::ACVZone_MeteorImpact()
{
    UE_LOG(LogTemp, Warning, TEXT("MeteorImpact: Constructor %s"), *GetName());

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

void ACVZone_MeteorImpact::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("MeteorImpact::BeginPlay %s, Class=%s, FallingVFX=%s, ImpactVFX=%s"),
        *GetName(),
        *GetNameSafe(GetClass()),
        *GetNameSafe(FallingVFX),
        *GetNameSafe(ImpactVFX));

    DrawDebugSphere(
        GetWorld(),
        GetActorLocation(),
        Collision->GetScaledSphereRadius(),
        24,
        FColor::Red,
        false,
        10.f,
        0,
        2.f
    );

    if (HasAuthority())
    {
        if (FallingVFX)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                FallingVFX,
                GetActorLocation() + FVector(0, 0, 800.f),
                FRotator::ZeroRotator,
                FVector(1.0f),
                true,
                true
            );
        }

        GetWorld()->GetTimerManager().SetTimer(
            ImpactTimer,
            this,
            &ACVZone_MeteorImpact::OnImpact,
            ImpactDelay,
            false
        );
    }
}

void ACVZone_MeteorImpact::OnImpact()
{
    if (!HasAuthority())
    {
        return;
    }

    // Collider 범위 내 대상에게 피해
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

void ACVZone_MeteorImpact::Multicast_PlayImpactVFX_Implementation()
{
    if (ImpactVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactVFX,
            GetActorLocation() + FVector(0, 0, -70.f),
            FRotator::ZeroRotator
        );
    }
}