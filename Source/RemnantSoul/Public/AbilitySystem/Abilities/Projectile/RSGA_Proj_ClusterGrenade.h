#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Projectile/RSGA_Proj_Base.h"
#include "RSGA_Proj_ClusterGrenade.generated.h"

UCLASS()
class REMNANTSOUL_API URSGA_Proj_ClusterGrenade : public URSGA_Proj_Base
{
	GENERATED_BODY()

public:
	URSGA_Proj_ClusterGrenade();

protected:
	/* 던지는 힘 (Physics Impulse) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Throw")
	float ThrowStrength = 1500.f;

	/* 던질 때 위로 올리는 각도 (degree) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Throw")
	float ThrowPitchAngle = 15.f;

	/* Physics가 아니면 ProjectileMovement로 던짐 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Throw")
	float FallbackInitialSpeed = 1500.f;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
