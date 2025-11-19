#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "CVGA_Install_HpPotion.generated.h"

//
// 설치형 HP 회복 포션 어빌리티
// - 플레이어 앞에 HpPotion 디바이스를 하나 설치한다.
//
UCLASS()
class CHRONOVEIL_API UCVGA_Install_HpPotion : public UCVGA_Install_Base
{
	GENERATED_BODY()
	
public:
    UCVGA_Install_HpPotion();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
};
