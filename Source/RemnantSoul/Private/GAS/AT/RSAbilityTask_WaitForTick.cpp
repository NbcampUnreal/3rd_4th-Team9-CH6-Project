// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AT/RSAbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSAbilityTask_WaitForTick)

URSAbilityTask_WaitForTick::URSAbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

URSAbilityTask_WaitForTick* URSAbilityTask_WaitForTick::WaitForTick(UGameplayAbility* OwningAbility)
{
	URSAbilityTask_WaitForTick* Task = NewAbilityTask<URSAbilityTask_WaitForTick>(OwningAbility);
	return Task;
}

void URSAbilityTask_WaitForTick::Activate()
{
	Super::Activate();
}

void URSAbilityTask_WaitForTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}