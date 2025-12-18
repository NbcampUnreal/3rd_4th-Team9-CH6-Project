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

	if (!Ability)
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
	TargetActorInstance = Cast<ARSTargetActor_SweepSingleCapsule>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (TargetActorInstance)
	{
		TargetActorInstance->SetShowDebug(false);
		TargetActorInstance->TargetDataReadyDelegate.AddUObject(this, &URSAbilityTask_SweepSingleCapsule::OnTargetDataReady);
	}
}

void URSAbilityTask_SweepSingleCapsule::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[SweepTask] ASC is NULL"));
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

	const FTransform SpawnTransform = Avatar->GetTransform();
	TargetActorInstance->FinishSpawning(SpawnTransform);

	ASC->SpawnedTargetActors.Push(TargetActorInstance);
	TargetActorInstance->StartTargeting(Ability);
	TargetActorInstance->ConfirmTargeting();
}

void URSAbilityTask_SweepSingleCapsule::OnDestroy(bool AbilityEnded)
{
	if (IsValid(TargetActorInstance) == true)
	{
		TargetActorInstance->Destroy();
	}

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
