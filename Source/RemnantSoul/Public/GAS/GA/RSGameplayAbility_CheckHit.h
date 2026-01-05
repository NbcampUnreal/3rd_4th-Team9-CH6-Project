#pragma once

#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_CheckHit.generated.h"

class UGameplayEffect;
class ARSTargetActor_SweepSingleCapsule;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_CheckHit : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	URSGameplayAbility_CheckHit();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnSweepSingleCapsuleResultReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> AttackDamageEffect;

	float CurrentLevel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> AttackBuffEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARSTargetActor_SweepSingleCapsule> TargetActorClass;

private:
	void SetupDamageSpec(FGameplayEffectSpecHandle& SpecHandle) const;

};
