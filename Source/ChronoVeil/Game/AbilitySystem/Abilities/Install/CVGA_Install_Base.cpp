#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"

UCVGA_Install_Base::UCVGA_Install_Base()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SpawnOffset = FVector(200.f, 0.f, 0.f);
}

void UCVGA_Install_Base::SpawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    if (!InstallClass)
    {
        return;
    }

    if (!HasAuthority(GetCurrentActivationInfoRef()))
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        FActorSpawnParameters Params;
        Params.Owner = GetOwningActorFromActorInfo();
        Params.Instigator = Cast<APawn>(GetOwningActorFromActorInfo());
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        World->SpawnActor<UCVGA_Install_Base>(InstallClass, SpawnLocation, SpawnRotation, Params);
    }
}
