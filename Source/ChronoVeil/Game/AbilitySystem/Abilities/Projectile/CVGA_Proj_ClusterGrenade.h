#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Projectile/CVGA_Proj_Base.h"
#include "CVGA_Proj_ClusterGrenade.generated.h"

class ACVProjectile_ClusterGrenade;
class UNiagaraSystem;
class USphereComponent;

UCLASS()
class CHRONOVEIL_API UCVGA_Proj_ClusterGrenade : public UCVGA_Proj_Base
{
	GENERATED_BODY()
    
public:
    UCVGA_Proj_ClusterGrenade();

protected:
    // 던지는 힘 (거리 결정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Throw")
    float ThrowStrength = 1500.f;

    // 던질 때 위로 올리는 각도 (도 단위)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Throw")
    float ThrowPitchAngle = 15.f;

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

};
