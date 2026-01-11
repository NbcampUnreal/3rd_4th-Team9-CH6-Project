#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Projectile/RSGA_Proj_Base.h"
#include "RSGA_Proj_Armageddon.generated.h"

UCLASS()
class REMNANTSOUL_API URSGA_Proj_Armageddon : public URSGA_Proj_Base
{
	GENERATED_BODY()

public:
	URSGA_Proj_Armageddon();

protected:
	/* ProjectileMovementComponent가 있으면 Velocity로 발사 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armageddon|Launch")
	float InitialSpeed = 1400.f;

	/* ProjectileMovementComponent가 없으면 Impulse로 발사 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armageddon|Launch")
	float ImpulseStrength = 1500.f;

	/* TriggerEventData의 Target/HIT를 이용해 조준 방향을 잡을지 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armageddon|Target")
	bool bUseTriggerTarget = true;

	/* 가능하면 HomingProjectile로 세팅 (ProjectileMovementComponent 필요) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armageddon|Target")
	bool bUseHomingIfPossible = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armageddon|Target")
	float HomingAcceleration = 8000.f;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	/* TriggerEventData에서 우선순위로 목표 위치를 뽑아냄 */
	bool ExtractTargetLocation(
		const FGameplayEventData* TriggerEventData,
		FVector& OutTargetLocation,
		TWeakObjectPtr<AActor>& OutTargetActor
	) const;
};
