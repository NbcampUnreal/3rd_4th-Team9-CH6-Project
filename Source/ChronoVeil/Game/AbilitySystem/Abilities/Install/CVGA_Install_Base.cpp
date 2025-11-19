#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "Game/Character/CVCharacter.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"

UCVGA_Install_Base::UCVGA_Install_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SpawnOffset = FVector(200.f, 0.f, 0.f);
    LifeSpan = 0.f;
}

ACVDevice_Base* UCVGA_Install_Base::SpawnDevice(
    TSubclassOf<ACVDevice_Base> InClass,
    const FTransform& SpawnTM,
    const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!InClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
    }

    // NetExecutionPolicy = ServerOnly 이고, ActivateAbility 쪽에서 HasAuthority 체크했다고 가정.
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ACVDevice_Base* Device = World->SpawnActor<ACVDevice_Base>(
        InClass, SpawnTM, Params);

    if (Device)
    {
        // ASC 소유자 → WorldObject로 연결
        Device->SetOwnerFromASC(Avatar);

        // Ability 단에서 Actor LifeSpan 오버라이드
        if (LifeSpan > 0.f)
        {
            Device->SetLifeSpan(LifeSpan);
        }
    }

    return Device;
}

ACVDevice_Base* UCVGA_Install_Base::SpawnSingleDevice(
    const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!DeviceClass || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();

    FVector Loc = Avatar->GetActorLocation()
        + Avatar->GetActorForwardVector() * SpawnOffset.X;
    Loc.Z += SpawnOffset.Z;

    const FRotator Rot = Avatar->GetActorRotation();
    const FTransform SpawnTM(Rot, Loc);

    return SpawnDevice(DeviceClass, SpawnTM, ActorInfo);
}

TArray<ACVDevice_Base*> UCVGA_Install_Base::SpawnMultipleDevices(
    const FGameplayAbilityActorInfo* ActorInfo)
{
    TArray<ACVDevice_Base*> OutDevices;

    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return OutDevices;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();

    for (const TSubclassOf<ACVDevice_Base>& ClassItem : DeviceClasses)
    {
        if (!*ClassItem)
        {
            continue;
        }

        FVector Loc = Avatar->GetActorLocation()
            + Avatar->GetActorForwardVector() * SpawnOffset.X;
        Loc.Z += SpawnOffset.Z;

        const FRotator Rot = Avatar->GetActorRotation();
        const FTransform SpawnTM(Rot, Loc);

        if (ACVDevice_Base* Device = SpawnDevice(ClassItem, SpawnTM, ActorInfo))
        {
            OutDevices.Add(Device);
        }
    }

    return OutDevices;
}
