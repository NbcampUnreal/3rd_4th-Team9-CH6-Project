#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_SweepSingleCapsule.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

UCLASS()
class REMNANTSOUL_API URSAbilityTask_SweepSingleCapsule : public UAbilityTask
{
	GENERATED_BODY()

public:
	URSAbilityTask_SweepSingleCapsule();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task",
		meta = (DisplayName = "WaitForSweepingSingleCapsule", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URSAbilityTask_SweepSingleCapsule* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ARSTargetActor_SweepSingleCapsule> TargetActorClass);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void ExternalCancel() override;

public:
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

protected:
	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle);

private:
	void CleanupTargetActor();

	UPROPERTY()
	TSubclassOf<class ARSTargetActor_SweepSingleCapsule> TargetActorClass;

	UPROPERTY()
	TObjectPtr<class ARSTargetActor_SweepSingleCapsule> TargetActorInstance;

	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	bool bCleanedUp = false;
};

