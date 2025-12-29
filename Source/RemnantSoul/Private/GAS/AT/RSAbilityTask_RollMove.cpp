// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AT/RSAbilityTask_RollMove.h"
#include "GameFramework/Character.h"

URSAbilityTask_RollMove* URSAbilityTask_RollMove::RollMove(
	UGameplayAbility* OwningAbility,
	FVector InDirection,
	float InDistance,
	float InDuration
)
{
	URSAbilityTask_RollMove* Task =
		NewAbilityTask<URSAbilityTask_RollMove>(OwningAbility);

	Task->MoveDirection = InDirection.GetSafeNormal();
	Task->TotalDistance = InDistance;
	Task->Duration = FMath::Max(InDuration, KINDA_SMALL_NUMBER);

	return Task;
}

void URSAbilityTask_RollMove::Activate()
{
	Super::Activate();

	bTickingTask = true;

	if (Ability && Ability->GetCurrentActorInfo())
	{
		Character = Cast<ACharacter>(
			Ability->GetCurrentActorInfo()->AvatarActor.Get()
		);
	}

	if (!Character.IsValid())
	{
		EndTask();
	}
}

void URSAbilityTask_RollMove::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!Character.IsValid())
	{
		EndTask();
		return;
	}

	ElapsedTime += DeltaTime;

	const float Alpha = ElapsedTime / Duration;
	const float TargetDistance = TotalDistance * Alpha;
	const float DeltaDistance = TargetDistance - MovedDistance;

	if (DeltaDistance > 0.f)
	{
		const FVector MoveDelta = MoveDirection * DeltaDistance;

		FHitResult Hit;
		Character->AddActorWorldOffset(MoveDelta, true, &Hit);

		if (Hit.bBlockingHit)
		{
			OnBlocked.Broadcast();
			EndTask();
			return;
		}

		MovedDistance += DeltaDistance;
	}

	if (ElapsedTime >= Duration)
	{
		OnFinished.Broadcast();
		EndTask();
	}
}

void URSAbilityTask_RollMove::OnDestroy(bool AbilityEnded)
{
	bTickingTask = false;
	Super::OnDestroy(AbilityEnded);
}
