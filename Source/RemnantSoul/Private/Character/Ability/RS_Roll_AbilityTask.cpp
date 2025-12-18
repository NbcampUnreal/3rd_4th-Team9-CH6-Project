// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/RS_Roll_AbilityTask.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"

URS_Roll_AbilityTask* URS_Roll_AbilityTask::PlayRollMontage(
	UGameplayAbility* OwningAbility,
	UAnimMontage* Montage
)
{
	if (!OwningAbility || !Montage)
	{
		return nullptr;
	}

	URS_Roll_AbilityTask* Task =
		NewAbilityTask<URS_Roll_AbilityTask>(OwningAbility);

	Task->RollMontage = Montage;
	return Task;
}

void URS_Roll_AbilityTask::Activate()
{
	if (!Ability)
	{
		EndTask();
		return;
	}

	AActor* AvatarActor = Ability->GetAvatarActorFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character)
	{
		EndTask();
		return;
	}

	AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		EndTask();
		return;
	}

	// 몽타주 재생
	AnimInstance->Montage_Play(RollMontage);

	// 종료 델리게이트 바인딩
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &URS_Roll_AbilityTask::OnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, RollMontage);
}

void URS_Roll_AbilityTask::OnMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted
)
{
	if (bInterrupted)
	{
		OnCancelled.Broadcast();
	}
	else
	{
		OnCompleted.Broadcast();
	}

	EndTask();
}

void URS_Roll_AbilityTask::OnDestroy(bool AbilityEnded)
{
	if (AnimInstance && RollMontage)
	{
		if (AnimInstance->Montage_IsPlaying(RollMontage))
		{
			AnimInstance->Montage_Stop(0.1f, RollMontage);
		}
	}

	Super::OnDestroy(AbilityEnded);
}