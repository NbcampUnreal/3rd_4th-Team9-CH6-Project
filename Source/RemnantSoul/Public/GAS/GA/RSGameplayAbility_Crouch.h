#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Crouch.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UAbilityTask_WaitInputRelease;
class URSAbilityTask_WaitForTick;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Crouch : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Crouch();

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

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UFUNCTION()
	void OnTick(float DeltaTime);

	bool CanCrouchNow() const;

	ACharacter* GetCharacterFromActorInfo() const;
};
