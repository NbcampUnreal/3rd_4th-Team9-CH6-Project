// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_RollMove.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRollMoveFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRollMoveDelegate);

UCLASS()
class REMNANTSOUL_API URSAbilityTask_RollMove : public UAbilityTask
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (DisplayName = "Roll Move",
				HidePin = "OwningAbility",
				DefaultToSelf = "OwningAbility"))
	static URSAbilityTask_RollMove* RollMove(
		UGameplayAbility* OwningAbility,
		FVector InDirection,
		float InDistance,
		float InDuration
	);

	UPROPERTY(BlueprintAssignable)
	FRollMoveFinished OnBlocked;   // 벽 충돌

	UPROPERTY(BlueprintAssignable)
	FRollMoveFinished OnFinished;

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

protected:
	FVector MoveDirection;
	float TotalDistance;
	float Duration;

	float ElapsedTime = 0.f;
	float MovedDistance = 0.f;

	TWeakObjectPtr<ACharacter> Character;
	

	
};
