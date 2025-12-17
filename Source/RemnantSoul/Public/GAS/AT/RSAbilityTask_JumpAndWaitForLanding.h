// RSAbilityTask_JumpAndWaitForLanding.h

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);

/**
 *
 */
UCLASS()
class REMNANTSOUL_API URSAbilityTask_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()

public:
	URSAbilityTask_JumpAndWaitForLanding();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (DisplayName = "JumpAndWaitForLanding", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URSAbilityTask_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);
	// 어빌리티 태스크 개체를 생성해 반환하는 함수는 static 멤버함수로 구현함.

	virtual void Activate() override;
	// 어빌리티 태스크 로직의 시작.

	virtual void OnDestroy(bool AbilityEnded) override;
	// 어빌리티 태스크 로직의 종료.

	FJumpAndWaitForLandingDelegate OnComplete;
	// 어빌리티 태스크가 완료되면 어빌리티에 알려주기 위한 델리게이트.

protected:
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

};
