//#pragma once
//
//#include "CoreMinimal.h"
//#include "AbilitySystem/Abilities/Zone/RSGA_Zone_Base.h"
//#include "RSGA_Zone_MeteorStrike.generated.h"
//
//class ARSZone_MeteorImpact;
//
//UCLASS()
//class REMNANTSOUL_API URSGA_Zone_MeteorStrike : public URSGA_Zone_Base
//{
//	GENERATED_BODY()
//
//public:
//	URSGA_Zone_MeteorStrike();
//
//protected:
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone|Meteor")
//	TSubclassOf<ARSZone_MeteorImpact> MeteorImpactZoneClass;
//
//protected:
//	virtual void ActivateAbility(
//		const FGameplayAbilitySpecHandle Handle,
//		const FGameplayAbilityActorInfo* ActorInfo,
//		const FGameplayAbilityActivationInfo ActivationInfo,
//		const FGameplayEventData* TriggerEventData
//	) override;
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "AbilitySystem/Abilities/Zone/RSGA_Zone_Base.h"
//#include "RSGA_Zone_MeteorStrike.generated.h"
//
//UCLASS()
//class REMNANTSOUL_API URSGA_Zone_MeteorStrike : public URSGA_Zone_Base
//{
//	GENERATED_BODY()
//
//public:
//	URSGA_Zone_MeteorStrike();
//
//protected:
//	// RSCharacter에 저장된 몽타주를 재생할지
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MeteorStrike|Montage")
//	bool bPlaySkillMontage = true;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MeteorStrike|Montage")
//	float MontagePlayRate = 1.0f;
//
//protected:
//	virtual void ActivateAbility(
//		const FGameplayAbilitySpecHandle Handle,
//		const FGameplayAbilityActorInfo* ActorInfo,
//		const FGameplayAbilityActivationInfo ActivationInfo,
//		const FGameplayEventData* TriggerEventData
//	) override;
//
//private:
//	void TryPlayMeteorStrikeMontage(const FGameplayAbilityActorInfo* ActorInfo) const;
//};

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Zone/RSGA_Zone_Base.h"
#include "RSGA_Zone_MeteorStrike.generated.h"

class ARSZone_MeteorImpact;
struct FGameplayAbilityActorInfo;

UCLASS()
class REMNANTSOUL_API URSGA_Zone_MeteorStrike : public URSGA_Zone_Base
{
	GENERATED_BODY()

public:
	URSGA_Zone_MeteorStrike();

protected:
	// 기존 방식 유지: MeteorImpactZoneClass로 스폰하던 흐름을 그대로 살림
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zone|Meteor")
	TSubclassOf<ARSZone_MeteorImpact> MeteorImpactZoneClass;

	// RSCharacter에 저장된 몽타주를 재생할지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MeteorStrike|Montage")
	bool bPlaySkillMontage = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MeteorStrike|Montage")
	float MontagePlayRate = 1.0f;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	void TryPlayMeteorStrikeMontage(const FGameplayAbilityActorInfo* ActorInfo) const;
};

