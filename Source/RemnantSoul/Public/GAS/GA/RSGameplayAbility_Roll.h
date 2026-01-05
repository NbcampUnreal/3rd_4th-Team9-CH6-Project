#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Roll.generated.h"

class ACharacter;
class APlayerController;

/**
 * RootMotion(State/BlendSpace) 기반 Roll Ability
 * - GA는 "상태 태그 + 방향 고정 + 종료 이벤트 대기"만 담당
 * - 실제 애니/이동은 AnimBP(State)에서 Root Motion from Everything으로 처리
 * - Roll 종료는 RSAnimNotify_RollEnd가 Event.Roll.End GameplayEvent를 쏴서 끝냄
 */
UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Roll : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Roll();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	// RollEnd GameplayEvent 수신
	UFUNCTION()
	void OnRollEndEventReceived(FGameplayEventData Payload);

	// 입력 기반 방향 각도(-180~180) 계산
	float ComputeRollDegreesFromInput(const ACharacter* Char) const;

	// 롤 중 이동 입력 누적 방지(선택)
	void SetMoveInputIgnored(bool bIgnored);

	// 롤 종료 순간 "툭" 전진 제거
	void FlushMovementAfterRoll(ACharacter* Char);

private:
	// 롤 중에만 켜고 끄는 입력 무시 컨트롤러 캐시
	TWeakObjectPtr<APlayerController> CachedPC;

	// 방어적 재진입 방지 플래그(태그로도 막히지만 안전장치)
	bool bRollingActive = false;

	float PostRollIgnoreMoveSeconds = 0.06f;   // 0.05~0.1 추천
	FTimerHandle TimerHandle_RestoreMoveInput;
};
