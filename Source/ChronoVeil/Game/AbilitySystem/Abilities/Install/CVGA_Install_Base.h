#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Install_Base.generated.h"

class ACVDevice_Base;
struct FGameplayAbilityActorInfo;

// 모든 설치물 GameplayAbility의 기초 클래스
UCLASS(Abstract)
class CHRONOVEIL_API UCVGA_Install_Base : public UCVGameplayAbility
{
    GENERATED_BODY()

public:
    UCVGA_Install_Base();

protected:
    // 단일 설치물 (BombMine, HpPotion 같은 것들)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
    TSubclassOf<ACVDevice_Base> DeviceClass;

    // 복수 설치물 (ToyTower + JumpPad 같이 여러 개 스폰하는 경우)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
    TArray<TSubclassOf<ACVDevice_Base>> DeviceClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
    FVector SpawnOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install")
    float LifeSpan = 0.f;

protected:
    // 공통 스폰 함수
    ACVDevice_Base* SpawnDevice(
        TSubclassOf<ACVDevice_Base> InClass,
        const FTransform& SpawnTM,
        const FGameplayAbilityActorInfo* ActorInfo);

    // 단일 설치물 스폰
    ACVDevice_Base* SpawnSingleDevice(
        const FGameplayAbilityActorInfo* ActorInfo);

    // 여러 설치물 스폰
    TArray<ACVDevice_Base*> SpawnMultipleDevices(
        const FGameplayAbilityActorInfo* ActorInfo);
};
