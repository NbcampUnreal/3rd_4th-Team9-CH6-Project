#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"

UCVGA_Move_Base::UCVGA_Move_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UCVGA_Move_Base::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    PerformMove(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 단발성 이동기면 바로 EndAbility를 발동시키자.
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UCVGA_Move_Base::PerformMove(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // 기본 구현 없음. 파생에서 override.
}
