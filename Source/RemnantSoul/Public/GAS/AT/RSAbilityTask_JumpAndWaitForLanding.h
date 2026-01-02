// RSAbilityTask_JumpAndWaitForLanding.h

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);

UCLASS()
class REMNANTSOUL_API URSAbilityTask_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()

public:
	URSAbilityTask_JumpAndWaitForLanding();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task",
		meta = (DisplayName = "JumpAndWaitForLanding", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URSAbilityTask_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void ExternalCancel() override;

public:
	UPROPERTY(BlueprintAssignable)
	FJumpAndWaitForLandingDelegate OnComplete;

protected:
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

private:
	void BindLanded();
	void UnbindLanded();

	TWeakObjectPtr<ACharacter> CachedCharacter;
	bool bBoundToLanded = false;
};

