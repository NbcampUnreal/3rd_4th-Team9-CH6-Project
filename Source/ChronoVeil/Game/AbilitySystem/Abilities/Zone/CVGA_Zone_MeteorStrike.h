#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Zone/CVGA_Zone_Base.h"
#include "CVGA_Zone_MeteorStrike.generated.h"

class ACVZone_MeteorImpact;

UCLASS()
class CHRONOVEIL_API UCVGA_Zone_MeteorStrike : public UCVGA_Zone_Base
{
    GENERATED_BODY()

public:
    UCVGA_Zone_MeteorStrike();

protected:
    // 떨어진 위치에 깔릴 장판(Zone) 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone|Meteor")
    TSubclassOf<ACVZone_MeteorImpact> MeteorImpactZoneClass;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};
