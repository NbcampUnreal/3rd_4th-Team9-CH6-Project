// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Jump.generated.h"

class UGameplayEffect;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Jump : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Jump();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;



#pragma region StaminaCost
protected:
	UFUNCTION()
	void HandleLanded();

	// 점프 발동 시 1회 적용할 스태미나 비용 GE
	UPROPERTY(EditDefaultsOnly, Category = "RS|Cost")
	TSubclassOf<UGameplayEffect> JumpStaminaCostEffectClass;
#pragma endregion
};
