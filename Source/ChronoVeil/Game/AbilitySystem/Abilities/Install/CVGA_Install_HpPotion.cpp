#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_HpPotion.h"
#include "Game/WorldObjects/Devices/CVDevice_HpPotion.h"

UCVGA_Install_HpPotion::UCVGA_Install_HpPotion()
{
    DeviceClass = ACVDevice_HpPotion::StaticClass();
}

void UCVGA_Install_HpPotion::ActivateAbility(
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
