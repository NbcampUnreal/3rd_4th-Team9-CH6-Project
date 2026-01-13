//#pragma once
//
//#include "Abilities/GameplayAbility.h"
//#include "RSGameplayAbility_Skill.generated.h"
//
//class UAnimMontage;
//
//UCLASS()
//class REMNANTSOUL_API URSGameplayAbility_Skill : public UGameplayAbility
//{
//	GENERATED_BODY()
//
//public:
//	URSGameplayAbility_Skill();
//
//public:
//	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
//
//	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
//
//protected:
//	UFUNCTION()
//	void OnCompleteCallback();
//
//	UFUNCTION()
//	void OnInterruptedCallback();
//
//protected:
//	UPROPERTY()
//	TObjectPtr<UAnimMontage> ActiveSkillActionMontage;
//
//};


// ===== RSGameplayAbility_Skill.h =====
#pragma once

#include "Abilities/GameplayAbility.h"
#include "RSGameplayAbility_Skill.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class ERSSkillVfxSpawnPolicy : uint8
{
	None,
	SpawnAtActor,
	AttachToMeshSocket
};

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Skill : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Skill();

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
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveSkillActionMontage;

	/* ===== VFX 설정 (에디터에서 GA마다 지정) ===== */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Skill|VFX")
	TObjectPtr<UNiagaraSystem> SkillVfx;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Skill|VFX")
	ERSSkillVfxSpawnPolicy VfxSpawnPolicy = ERSSkillVfxSpawnPolicy::AttachToMeshSocket;

	// AttachToMeshSocket 일 때 사용할 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Skill|VFX")
	FName VfxAttachSocketName = NAME_None;

	// SpawnAtActor 일 때 오프셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Skill|VFX")
	FVector VfxWorldOffset = FVector::ZeroVector;

	// EndAbility에서 정리하기 위해 유지
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> SpawnedVfxComponent = nullptr;

private:
	void SpawnSkillVfx(const FGameplayAbilityActorInfo* ActorInfo);
	void CleanupSkillVfx();
};
