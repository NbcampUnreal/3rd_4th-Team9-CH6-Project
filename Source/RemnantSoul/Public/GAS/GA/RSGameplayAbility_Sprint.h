#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Sprint.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Sprint : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Sprint();

protected:
	// UGameplayAbility
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

	// 핵심: Input Released가 들어오면 여기로 들어온다(AbilitySpecInputReleased 경유)
	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	bool CanSprintNow() const;

	ACharacter* GetCharacterFromActorInfo() const;
	UCharacterMovementComponent* GetMovementFromActorInfo() const;

	void ApplySprintSpeed(UCharacterMovementComponent* MoveComp);
	void RestoreWalkSpeed(UCharacterMovementComponent* MoveComp);

private:
	// Sprint 적용 전 WalkSpeed 캐시
	float CachedWalkSpeed = 0.f;

	// 설정값: (1) 고정 SprintSpeed
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float SprintWalkSpeed = 600.f;

	// 설정값: (2) 배수 방식도 가능 (지금은 사용하고 있지 않음.)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float SprintSpeedMultiplier = 1.5f;

	// 정책: 고정값 사용 여부
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	bool bUseFixedSprintSpeed = true;
};
