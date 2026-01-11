//#pragma once
//
//#include "CoreMinimal.h"
//#include "AbilitySystem/Abilities/Install/RSGA_Install_Base.h"
//#include "RSGA_Install_Obstacle.generated.h"
//
//UCLASS()
//class REMNANTSOUL_API URSGA_Install_Obstacle : public URSGA_Install_Base
//{
//	GENERATED_BODY()
//
//public:
//	URSGA_Install_Obstacle();
//
//protected:
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|Obstacle")
//	TSubclassOf<AActor> ObstacleClass;
//
//protected:
//	virtual void ActivateAbility(
//		const FGameplayAbilitySpecHandle Handle,
//		const FGameplayAbilityActorInfo* ActorInfo,
//		const FGameplayAbilityActivationInfo ActivationInfo,
//		const FGameplayEventData* TriggerEventData
//	) override;
//};


#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Install/RSGA_Install_Base.h"
#include "RSGA_Install_Obstacle.generated.h"

class UAnimMontage;

UCLASS()
class REMNANTSOUL_API URSGA_Install_Obstacle : public URSGA_Install_Base
{
	GENERATED_BODY()

public:
	URSGA_Install_Obstacle();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|Obstacle")
	TSubclassOf<AActor> ObstacleClass;

	// RSCharacter에 세팅된 Skill_ObstacleMontage를 재생할지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|Obstacle|Montage")
	bool bPlaySkillMontage = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Install|Obstacle|Montage")
	float MontagePlayRate = 1.0f;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	void TryPlayObstacleMontage(const FGameplayAbilityActorInfo* ActorInfo) const;
};
