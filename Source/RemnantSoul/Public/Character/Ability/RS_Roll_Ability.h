// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RS_Roll_Ability.generated.h"

class UAnimMontage;
class UGameplayEffect;
class URSAbilityTask_RollMove;
class UAbilityTask_WaitGameplayEvent;
UCLASS()
class REMNANTSOUL_API URS_Roll_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URS_Roll_Ability();

protected:

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	) override;

	
	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> RollCostGE;

	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> InvincibleGE;

	UPROPERTY(EditDefaultsOnly, Category="Roll|Effect")
	TSubclassOf<UGameplayEffect> RollCooldownGE;
	

	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float RollDistance = 600.f;

	/** Roll 지속 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float RollDuration = 0.6f;
	
	float CachedRollAngle = 0.f;

	//이벤트 기반 종료 테스트 코드
	UFUNCTION()
	void OnRollEndEvent(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category="Roll|Event")
	FGameplayTag RollEndEventTag;

	UFUNCTION()
	void OnRollBlocked();
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEndEvent;
	
};
