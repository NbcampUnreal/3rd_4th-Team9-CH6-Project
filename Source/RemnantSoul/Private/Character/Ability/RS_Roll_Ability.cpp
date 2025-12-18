// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/RS_Roll_Ability.h"
#include "AbilitySystemComponent.h"
#include "Character/Ability/RS_Roll_AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


URS_Roll_Ability::URS_Roll_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	//어빌리티를 실행할 태그 설정.
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Roll")));

	//이 태그가 있을 시 어빌리티를 실행하지 않음.
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Roll.CoolDown")));

	//이 어빌리티의 소유자가 State.Dead 태그를 소유할 시 실행 불가, 플레이어 캐릭터 사망 시 어빌리티 실행을 막는 구조로 구상 중이며 추후 리스폰 완성되면 최종적으로 확립할 예정.
	//ActivationBlockedTags.AddTag(
	//	FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));

	CostGameplayEffectClass = RollCostGE;
	CooldownGameplayEffectClass = RollCooldownGE;
}

void URS_Roll_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	//어빌리티 실행 조건(스태미나, 내부 쿨타임) 둘 중 하나라도 만족하지 못할 경우 어빌리티 실행 실패.
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	//ASC가 없을 경우 어빌리티 종료.
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC || !RollMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	if (InvincibleGE)
	{
		ASC->ApplyGameplayEffectToSelf(
			InvincibleGE.GetDefaultObject(),
			1.f,
			ASC->MakeEffectContext()
		);
	}


	//애니메이션 파트
	URS_Roll_AbilityTask* RollTask =
		URS_Roll_AbilityTask::PlayRollMontage(this, RollMontage);

	if (!RollTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	RollTask->OnCompleted.AddDynamic(this, &URS_Roll_Ability::OnMontageCompleted);
	RollTask->OnCancelled.AddDynamic(this, &URS_Roll_Ability::OnMontageCancelled);

	RollTask->ReadyForActivation();
}

void URS_Roll_Ability::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void URS_Roll_Ability::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void URS_Roll_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		false,
		bWasCancelled
	);
}
