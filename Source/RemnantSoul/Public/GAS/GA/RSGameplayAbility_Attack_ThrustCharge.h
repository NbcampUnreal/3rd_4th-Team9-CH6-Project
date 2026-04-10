#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Attack_ThrustCharge.generated.h"

class UGameplayEffect;
class UAnimMontage;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Attack_ThrustCharge : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Attack_ThrustCharge();

public:
	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

public:
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

protected:
	UFUNCTION()
	void OnChargeCompleted();

	UFUNCTION()
	void OnChargeCanceled();

	UFUNCTION()
	void OnReleaseCompleted();

	UFUNCTION()
	void OnReleaseCanceled();

private:
	void StartChargeTimer();
	void TryPlayChargeMontage();
	void TryPlayReleaseMontage();
	void SendCheckHitEvent() const;
	float GetCurrentChargeSeconds() const;

private:
	// 입력 상태
	bool bIsCharging = false;
	bool bWantsRelease = false;

	// 차지 시간 측정
	float ChargeStartTime = 0.f;

	FTimerHandle ChargeTimerHandle;

	// 캐시
	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedChargeMontage = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> CachedReleaseMontage = nullptr;

	// 차지 최소/최대 (길게 눌러도 1회만 나가도록)
	UPROPERTY(EditDefaultsOnly, Category = "RS|ThrustCharge|Charge")
	float MinChargeSeconds = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "RS|ThrustCharge|Charge")
	float MaxChargeSeconds = 1.50f;

	// 차지 상태에서 주기적으로 릴리즈 여부 체크
	UPROPERTY(EditDefaultsOnly, Category = "RS|ThrustCharge|Charge")
	float ChargePollInterval = 0.02f;

	// 릴리즈(강찌르기) 몽타주 재생 속도
	UPROPERTY(EditDefaultsOnly, Category = "RS|ThrustCharge|Release")
	float ReleasePlayRate = 0.85f;

#pragma region StaminaCost
	UPROPERTY(EditDefaultsOnly, Category = "RS|Cost")
	TSubclassOf<UGameplayEffect> StaminaCostEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Cost")
	float StaminaCost = 20.0f;
#pragma endregion
};
