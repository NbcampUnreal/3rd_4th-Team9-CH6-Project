#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RSGamePlayAbility_Interact.generated.h"

/**
 * GA_Interact:
 * - 카메라 기준 라인트레이스
 * - 히트 액터가 IInteractable이면 CanInteract/Interact 호출
 */
UCLASS()
class REMNANTSOUL_API URSGamePlayAbility_Interact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGamePlayAbility_Interact();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	bool DoInteractTrace(
		const FGameplayAbilityActorInfo* ActorInfo,
		FHitResult& OutHit
	) const;

private:
	UPROPERTY(EditDefaultsOnly, Category="Interact|Trace", meta=(ClampMin="50.0"))
	float TraceDistance = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category="Interact|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category="Interact|Trace")
	bool bTraceComplex = false;
};
