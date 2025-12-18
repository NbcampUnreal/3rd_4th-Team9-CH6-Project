#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_SweepSingleCapsule.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

class ARSTargetActor_SweepSingleCapsule;

UCLASS()
class REMNANTSOUL_API URSAbilityTask_SweepSingleCapsule : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	URSAbilityTask_SweepSingleCapsule();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (DisplayName = "WaitForSweepingSingleCapsule", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URSAbilityTask_SweepSingleCapsule* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ARSTargetActor_SweepSingleCapsule> TargetActorClass);

	virtual void Activate() override;

	void SpawnAndInitializeTargetActor();

	void FinalizeTargetActor();

	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<ARSTargetActor_SweepSingleCapsule> TargetActorClass;

	UPROPERTY()
	TObjectPtr<ARSTargetActor_SweepSingleCapsule> TargetActorInstance;
};
