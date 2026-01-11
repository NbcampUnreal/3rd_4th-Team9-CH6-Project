#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility.generated.h"

class ARSCharacter;
class UAbilitySystemComponent;

UCLASS()
class REMNANTSOUL_API URSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility();

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

protected:
	ARSCharacter* GetRSCharacter(const FGameplayAbilityActorInfo* ActorInfo) const;
	UAbilitySystemComponent* GetRSASC(const FGameplayAbilityActorInfo* ActorInfo) const;

	bool HasAuthority(const FGameplayAbilityActivationInfo& ActivationInfo) const;
};
