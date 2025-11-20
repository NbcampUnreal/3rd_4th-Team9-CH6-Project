#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/CVGameplayAbility.h"
#include "CVGA_Proj_Base.generated.h"

class ACVProjectile_Base;

UCLASS(Abstract)
class CHRONOVEIL_API UCVGA_Proj_Base : public UCVGameplayAbility
{
	GENERATED_BODY()
	
public:
    UCVGA_Proj_Base();

protected:
    // 기본으로 쏠 투사체 클래스 (단발형 : 하나의 객체만 생성하고 싶을 때 설정.)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<ACVProjectile_Base> ProjectileClass;

    // 여러개의 투사체 클래스 생성 (연발형 : 여러개의 객체가 생성됐으면 좋겠을때 설정.)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    TArray<TSubclassOf<ACVProjectile_Base>> ProjectileClasses;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    FVector SpawnOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
    float LifeSpan = 0.f;

protected:
    // 공통 투사체 스폰 헬퍼
    // - InClass: 실제로 스폰할 투사체 클래스
    // - SpawnTM: 월드에 배치할 트랜스폼
    // - ActorInfo: Ability 시전자 정보 (Avatar/Controller 등 포함)
    ACVProjectile_Base* SpawnProjectile(
        TSubclassOf<ACVProjectile_Base> InClass,
        const FTransform& SpawnTM,
        const FGameplayAbilityActorInfo* ActorInfo
    );
};
