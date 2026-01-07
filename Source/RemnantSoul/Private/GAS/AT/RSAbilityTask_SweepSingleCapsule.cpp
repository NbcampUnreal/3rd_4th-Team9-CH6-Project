// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AT/RSAbilityTask_SweepSingleCapsule.h"
#include "GAS/TA/RSTargetActor_SweepSingleCapsule.h"
#include "AbilitySystemComponent.h"

URSAbilityTask_SweepSingleCapsule::URSAbilityTask_SweepSingleCapsule()
{
}

URSAbilityTask_SweepSingleCapsule* URSAbilityTask_SweepSingleCapsule::CreateTask(UGameplayAbility* OwningAbility,
	TSubclassOf<ARSTargetActor_SweepSingleCapsule> TargetActorClass)
{
	URSAbilityTask_SweepSingleCapsule* NewTask = NewAbilityTask<URSAbilityTask_SweepSingleCapsule>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void URSAbilityTask_SweepSingleCapsule::Activate()
{
	Super::Activate();

	if (!Ability || !Ability->GetCurrentActorInfo())
	{
		EndTask();
		return;
	}

	if (!TargetActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] TargetActorClass is NULL"));
		EndTask();
		return;
	}

	// UAbilityTask::AbilitySystemComponent는 상황에 따라 세팅이 늦거나 비어있을 수 있음
	CachedASC = Ability->GetCurrentActorInfo()->AbilitySystemComponent.Get();
	if (!CachedASC.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] ASC is NULL (ActorInfo)"));
		EndTask();
		return;
	}

	SpawnAndInitializeTargetActor();

	if (!IsValid(TargetActorInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] Failed to spawn TargetActorInstance"));
		EndTask();
		return;
	}

	FinalizeTargetActor();
	SetWaitingOnAvatar();
}

void URSAbilityTask_SweepSingleCapsule::SpawnAndInitializeTargetActor()
{
	UWorld* World = Ability ? Ability->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] World is NULL"));
		return;
	}

	// Deferred spawn: 타입을 바로 TargetActor 타입으로 스폰 (캐스팅 실패 방지)
	TargetActorInstance = World->SpawnActorDeferred<ARSTargetActor_SweepSingleCapsule>(
		TargetActorClass,
		FTransform::Identity,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!IsValid(TargetActorInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] SpawnActorDeferred failed"));
		return;
	}

	TargetActorInstance->SetShowDebug(false);

	// 태스크가 먼저 죽을 수 있으니, OnDestroy에서 반드시 해제할 수 있게 AddUObject 유지 + Cleanup에서 RemoveAll 처리
	TargetActorInstance->TargetDataReadyDelegate.AddUObject(this, &ThisClass::OnTargetDataReady);
}

void URSAbilityTask_SweepSingleCapsule::OnDestroy(bool AbilityEnded)
{
	CleanupTargetActor();
	Super::OnDestroy(AbilityEnded);
}

void URSAbilityTask_SweepSingleCapsule::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}

void URSAbilityTask_SweepSingleCapsule::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] ASC is NULL in Finalize"));
		EndTask();
		return;
	}

	if (!IsValid(TargetActorInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] TargetActorInstance is NULL in Finalize"));
		EndTask();
		return;
	}

	AActor* Avatar = ASC->GetAvatarActor();
	if (!IsValid(Avatar))
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] Avatar is NULL"));
		EndTask();
		return;
	}

	TargetActorInstance->FinishSpawning(Avatar->GetTransform());

	// SpawnedTargetActors에 넣었으면 반드시 Cleanup에서 제거해야 함
	ASC->SpawnedTargetActors.Add(TargetActorInstance);

	TargetActorInstance->StartTargeting(Ability);

	// ConfirmTargeting() -> ConfirmTargetingAndContinue() 호출
	TargetActorInstance->ConfirmTargeting();
}

void URSAbilityTask_SweepSingleCapsule::ExternalCancel()
{
	CleanupTargetActor();
	Super::ExternalCancel();
	EndTask();
}

void URSAbilityTask_SweepSingleCapsule::CleanupTargetActor()
{
	if (bCleanedUp)
	{
		return;
	}
	bCleanedUp = true;

	UAbilitySystemComponent* ASC = CachedASC.Get();

	if (IsValid(TargetActorInstance))
	{
		TargetActorInstance->TargetDataReadyDelegate.RemoveAll(this);

		if (ASC)
		{
			ASC->SpawnedTargetActors.Remove(TargetActorInstance);
		}

		TargetActorInstance->Destroy();
		TargetActorInstance = nullptr;
	}
}

