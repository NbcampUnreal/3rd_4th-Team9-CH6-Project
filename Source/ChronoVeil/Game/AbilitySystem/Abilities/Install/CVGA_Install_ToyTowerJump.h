#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "CVGA_Install_ToyTowerJump.generated.h"

//
// 설치형 장난감타워랑 점프패드를 설치하는 어빌리티
// - 플레이어 앞에 장난감타워랑 점프패드를 설치 설치한다.
//
UCLASS()
class CHRONOVEIL_API UCVGA_Install_ToyTowerJump : public UCVGA_Install_Base
{
	GENERATED_BODY()
	
public:
    UCVGA_Install_ToyTowerJump();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

protected:
    void SpawnObstacleDevices(const FGameplayAbilityActorInfo* ActorInfo);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|ToyTower")
    float JumpPadBackOffset = -500.0f;
};
