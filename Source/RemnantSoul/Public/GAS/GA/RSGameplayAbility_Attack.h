// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Attack.generated.h"

class UGameplayEffect;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Attack();

public:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnCanceled();

	FName GetNextAnimMontageSection();

	void StartComboTimer();

	void CheckComboInput();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	uint8 CurrentCombo = 0;

	FTimerHandle ComboTimerHandle;

	bool IsNextComboInputPressed = false;

#pragma region ComboSpeed
	float GetPlayRateForCombo(uint8 Combo) const;
	FName GetSectionNameForCombo(uint8 Combo) const;

	void ApplyMontagePlayRate(UAnimMontage* Montage, float PlayRate);
#pragma endregion

#pragma region StaminaCost
	UPROPERTY(EditDefaultsOnly, Category = "RS|Cost")
	TSubclassOf<UGameplayEffect> StaminaCostEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cost")
	float StaminaCost = 15.0f;
#pragma endregion

};
