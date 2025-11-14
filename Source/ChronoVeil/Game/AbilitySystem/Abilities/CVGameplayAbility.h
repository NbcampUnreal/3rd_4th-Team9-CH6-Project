#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CVGameplayAbility.generated.h"

class ACVCharacter;
class UCVAbilitySystemComponent;

UCLASS()
class CHRONOVEIL_API UCVGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCVGameplayAbility();

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
	ACVCharacter* GetCVCharacter(const FGameplayAbilityActorInfo* ActorInfo) const;

	UCVAbilitySystemComponent* GetCVASC(const FGameplayAbilityActorInfo* ActorInfo) const;

	bool HasAuthority(const FGameplayAbilityActivationInfo& ActivationInfo) const;





};

