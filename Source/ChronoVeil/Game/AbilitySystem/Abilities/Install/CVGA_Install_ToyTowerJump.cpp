#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_ToyTowerJump.h"
#include "Game/WorldObjects/Devices/CVDevice_JumpPad.h"
#include "Game/WorldObjects/Devices/CVDevice_ToyTower.h"

UCVGA_Install_ToyTowerJump::UCVGA_Install_ToyTowerJump()
{
    SpawnOffset = FVector(1200.0f, 0.f, 0.f);
    LifeSpan = 10.f;
    // DeviceClasses 는 BP에서 ToyTower / JumpPad 넣어도 되고,
    // C++에서 직접 AddDefaulted()로 채워도 됨.
}

void UCVGA_Install_ToyTowerJump::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!HasAuthority(ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    SpawnObstacleDevices(ActorInfo);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UCVGA_Install_ToyTowerJump::SpawnObstacleDevices(
    const FGameplayAbilityActorInfo* ActorInfo)
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        return;
    }

    AActor* Avatar = ActorInfo->AvatarActor.Get();

    for (const TSubclassOf<ACVDevice_Base>& DeviceClassItem : DeviceClasses)
    {
        if (!*DeviceClassItem)
        {
            continue;
        }

        FVector SpawnLocation =
            Avatar->GetActorLocation()
            + Avatar->GetActorForwardVector() * SpawnOffset.X;
        SpawnLocation.Z += SpawnOffset.Z;

        // 점프패드는 캐릭터 뒤에 스폰
        if (DeviceClassItem->IsChildOf(ACVDevice_JumpPad::StaticClass()))
        {
            SpawnLocation += Avatar->GetActorForwardVector() * JumpPadBackOffset;
        }

        const FRotator SpawnRot = Avatar->GetActorRotation();
        const FTransform SpawnTM(SpawnRot, SpawnLocation);

        SpawnDevice(DeviceClassItem, SpawnTM, ActorInfo);
    }
}
