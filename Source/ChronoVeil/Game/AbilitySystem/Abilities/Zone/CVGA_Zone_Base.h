#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Zone_Base.generated.h"

class ACVZone_Base;
struct FGameplayAbilityActorInfo;

UCLASS(Abstract)
class CHRONOVEIL_API UCVGA_Zone_Base : public UCVGameplayAbility
{
    GENERATED_BODY()

public:
    UCVGA_Zone_Base();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone")
    TSubclassOf<ACVZone_Base> ZoneClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone")
    TArray<TSubclassOf<ACVZone_Base>> ZoneClasses;

    // 캐릭터 기준 스폰 오프셋 (Forward / Right / Up) : 축(기즈모임.)이라고 생각하면 됨.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    FVector SpawnOffset = FVector(200.f, 0.f, 0.f);

    // 0보다 크면 존에 SetLifeSpan 적용 (존 내부 LifeTime과는 별개로, actor 자체 수명)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    float LifeSpan = 0.f;

protected:
    // 공통 존 스폰 헬퍼
    // - InClass: 실제로 생성할 존 클래스
    // - SpawnTM: 월드에 놓을 트랜스폼
    // - ActorInfo: 시전자 정보 (Avatar/Controller 등)
    ACVZone_Base* SpawnZone(
        TSubclassOf<ACVZone_Base> InClass,
        const FTransform& SpawnTM,
        const FGameplayAbilityActorInfo* ActorInfo
    );

    // 단일 존 스폰 (ex. 롤 미스포츈 e스킬 : 총알은 비를 타고(스킬이름)생각하면 됌.)
    ACVZone_Base* SpawnSingleZone(
        const FGameplayAbilityActorInfo* ActorInfo);

    // 여러 존 스폰 (ex. 롤초토화봇 모드 초가스 궁극기 : 장판 여러개 생성)
    TArray<ACVZone_Base*> SpawnMultipleZones(
        const FGameplayAbilityActorInfo* ActorInfo);
};
