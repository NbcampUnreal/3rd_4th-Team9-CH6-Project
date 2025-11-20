#pragma once

#include "CoreMinimal.h"
#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_Base.h"
#include "CVGA_Install_AffectionDoll.generated.h"

class ACVDevice_AffectionDoll;

//
// 애정인형 설치 어빌리티
// - 캐릭터 앞 바닥에 애정인형 디바이스를 설치
// - 실제 연출/풀링 로직은 ACVDevice_AffectionDoll 쪽에 구현
//
UCLASS()
class CHRONOVEIL_API UCVGA_Install_AffectionDoll : public UCVGA_Install_Base
{
	GENERATED_BODY()

public:
	UCVGA_Install_AffectionDoll();

protected:
	// 설치할 인형 클래스 (BP 로 BP_CVDevice_AffectionDoll 지정 권장)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|AffectionDoll")
	TSubclassOf<ACVDevice_AffectionDoll> AffectionDollClass;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};