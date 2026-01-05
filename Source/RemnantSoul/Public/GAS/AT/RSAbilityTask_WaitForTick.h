// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RSAbilityTask_WaitForTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

UCLASS()
class REMNANTSOUL_API URSAbilityTask_WaitForTick : public UAbilityTask
{
	GENERATED_BODY()

public:
	URSAbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static URSAbilityTask_WaitForTick* WaitForTick(UGameplayAbility* OwningAbility);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
};
