#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_SuperJump.generated.h"

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_SuperJump : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_SuperJump();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo
	) override;

protected:
	/** Called when landing is detected */
	UFUNCTION()
	void OnLanded();

	/** 점프 세기 (에디터에서 조정 가능) */
	UPROPERTY(EditDefaultsOnly, Category = "SuperJump")
	float SuperJumpStrength = 1200.f;
};
