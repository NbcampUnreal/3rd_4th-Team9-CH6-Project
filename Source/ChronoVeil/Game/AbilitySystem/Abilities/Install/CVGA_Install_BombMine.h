#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "CVGA_Install_BombMine.generated.h"


class UNiagaraSystem;

//
// 설치형 지뢰폭탄(BombMine) 어빌리티
// - 플레이어 앞에 BombMine 디바이스를 하나 설치한다.
//
UCLASS()
class CHRONOVEIL_API UCVGA_Install_BombMine : public UCVGA_Install_Base
{
	GENERATED_BODY()
	
public:
    UCVGA_Install_BombMine();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
};
