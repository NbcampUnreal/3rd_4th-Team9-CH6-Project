#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_BombMine.h"
#include "Game/WorldObjects/Devices/CVDevice_BombMine.h"

UCVGA_Install_BombMine::UCVGA_Install_BombMine()
{
    DeviceClass = ACVDevice_BombMine::StaticClass();
}

void UCVGA_Install_BombMine::ActivateAbility(
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

    SpawnSingleDevice(ActorInfo);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
