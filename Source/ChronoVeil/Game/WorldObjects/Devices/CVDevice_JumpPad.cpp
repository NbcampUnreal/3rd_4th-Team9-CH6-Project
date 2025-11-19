#include "Game/WorldObjects/Devices/CVDevice_JumpPad.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ACVDevice_JumpPad::ACVDevice_JumpPad()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->InitBoxExtent(FVector(100.f, 100.f, 50.f));
    BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // CVDevice_Base에서 Collision을 Root로 쓰고 있으므로, 그 밑에 붙인다.
    BoxCollision->SetupAttachment(GetRootComponent());

    BoxCollision->OnComponentBeginOverlap.AddDynamic(
        this, &ACVDevice_JumpPad::OnOverlap
    );
}

void ACVDevice_JumpPad::OnOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 점프/이동 같은 건 서버가 권위 갖는 쪽으로 갈 거라면 : 점프/이동 판정은 서버에서 하는게 맞을듯? 왜냐하면 점프랑 이동은 동기가 정확히 게임밸런스때문에 이루어져야함. 
    if (!HasAuthority())
    {
        return;
    }

    if (!OtherActor)
    {
        return;
    }

    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }

    // 기존 속도는 유지하고, Z값만 갱신함.,
    const FVector LaunchVector(0.f, 0.f, LaunchVelocity);

    Character->LaunchCharacter(
        LaunchVector,
        false,  // XY Override 안 함
        true    // Z값만 적용함.
    );

    // 나중에 팀/시전자 기준 추가 로직이 필요하면
    // GetOwningController() / GetOwningPawn() 사용 가능하니까 추후에 추가할까 고민중.
}
