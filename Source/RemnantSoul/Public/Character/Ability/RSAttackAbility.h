// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    	URSAttackAbility();
    	
protected:
	/* 공격 파라미터 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
	//TODO: 실제 데미지 구조가 반영되지 않음, 추후에 로직 추가 필요.

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	void PerformAttackTrace(const FGameplayAbilityActorInfo* ActorInfo,float AttackDamageValue);
	
};
