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

	AActor* Avatar = GetAvatarActor();
	ACharacter* C = Cast<ACharacter>(Avatar);
	if (!IsValid(C))
	{
		EndTask();
		return;
	}

	Character = C;

	// Duration 가드
	Duration = FMath::Max(Duration, KINDA_SMALL_NUMBER);

	SetWaitingOnAvatar();
}


void URSAbilityTask_RollMove::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	ACharacter* C = Character.Get();
	if (!IsValid(C))
	{
		EndTask();
		return;
	}

	ElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);
	const float TargetDistance = TotalDistance * Alpha;
	const float DeltaDistance = TargetDistance - MovedDistance;

	if (DeltaDistance > 0.f)
	{
		const FVector MoveDelta = MoveDirection * DeltaDistance;

		FHitResult Hit;
		C->AddActorWorldOffset(MoveDelta, true, &Hit);

		if (Hit.bBlockingHit)
		{
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnBlocked.Broadcast();
			}
			EndTask();
			return;
		}

		MovedDistance += DeltaDistance;
	}

	if (ElapsedTime >= Duration)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}
		EndTask();
	}
}

void URSAbilityTask_RollMove::OnDestroy(bool AbilityEnded)
{
	Cleanup();
	Super::OnDestroy(AbilityEnded);
}

void URSAbilityTask_RollMove::ExternalCancel()
{
	Cleanup();
	Super::ExternalCancel();
	EndTask();
}

void URSAbilityTask_RollMove::Cleanup()
{
	bTickingTask = false;
	Character = nullptr;
}
