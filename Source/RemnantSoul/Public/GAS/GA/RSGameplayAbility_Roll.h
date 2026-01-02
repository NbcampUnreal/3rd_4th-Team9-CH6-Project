#pragma once

#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Roll.generated.h"

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
	UFUNCTION()
	void OnRollEndEventReceived(FGameplayEventData Payload);

	float ComputeRollDegreesFromInput(const ACharacter* Char) const;

	// Roll한뒤에 앞으로 옮겨지는 버그 방지
	TWeakObjectPtr<APlayerController> CachedPC;

	void SetMoveInputIgnored(bool bIgnored);
	void FlushMovementAfterRoll(ACharacter* Char);
};
