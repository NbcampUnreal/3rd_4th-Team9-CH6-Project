#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AttributeSet.h"
#include "RSGameplayAbility_Sprint_Active.generated.h"

class URSAbilityTask_WaitForTick;
class UGameplayEffect;
class ACharacter;
class UCharacterMovementComponent;
class UAbilityTask_WaitInputRelease;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Sprint_Active : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Sprint_Active();

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
	void OnTick(float DeltaTime);

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void OnSprintCommitTick();

	bool IsStillForwardMoving_ServerAuth() const;
	bool IsStillForwardMoving_ClientApprox() const;

	ACharacter* GetCharacterFromActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float ForwardThreshold = 0.25f;

	// MaxWalkSpeed 기반 빠른 구현(추천)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float SprintMaxWalkSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float CommitInterval = 0.1f;

	// (선택) 종료 후 회복/블락 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	TSubclassOf<UGameplayEffect> RecoveryBlockEffectClass;

	// (선택) 자원 커밋/검사용 Attribute (없으면 무시)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	FGameplayAttribute SprintCostAttribute;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float MinRemainValueToKeepSprint = 1.f;

private:
	FTimerHandle SprintTimerHandle;

	float CachedOriginalMaxWalkSpeed = 0.f;
	bool bSpeedOverridden = false;
};
