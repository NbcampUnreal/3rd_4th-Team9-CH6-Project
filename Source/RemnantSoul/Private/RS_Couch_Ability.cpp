// Fill out your copyright notice in the Description page of Project Settings.


#include "RS_Couch_Ability.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/LogMacros.h"



URS_Couch_Ability::URS_Couch_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	CrouchRootTag  = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch"));
	CrouchStartTag = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.Start"));
	CrouchLoopTag  = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.Loop"));
	CrouchEndTag   = FGameplayTag::RequestGameplayTag(TEXT("State.Crouch.End"));

	StartFinishedEventTag =FGameplayTag::RequestGameplayTag(TEXT("Event.Crouch.StartFinished"));
	EndFinishedEventTag =FGameplayTag::RequestGameplayTag(TEXT("Event.Crouch.EndFinished"));
	EndRequestedEventTag =FGameplayTag::RequestGameplayTag(TEXT("Event.Crouch.EndRequested"));

	ActivationOwnedTags.AddTag(CrouchRootTag);
}

void URS_Couch_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// 기존 태그 초기화
	ASC->RemoveLooseGameplayTag(CrouchStartTag);
	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->RemoveLooseGameplayTag(CrouchEndTag);

	// Start 진입
	ASC->AddLooseGameplayTag(CrouchStartTag);

	// Start 종료 대기
	WaitStartFinished = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StartFinishedEventTag);
	WaitStartFinished->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnStartFinished);
	WaitStartFinished->ReadyForActivation();


	// Loop 종료 대기
	WaitEndFinishedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndRequestedEventTag);
	WaitEndFinishedTask->EventReceived.AddDynamic(this, &URS_Couch_Ability::HandleEndRequested);
	WaitEndFinishedTask->ReadyForActivation();

	
	// EndFinished 이벤트 대기
	WaitEndFinishedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndFinishedEventTag);
	WaitEndFinishedTask->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnEndFinished);
	WaitEndFinishedTask->ReadyForActivation();
}

void URS_Couch_Ability::OnStartFinished(FGameplayEventData Payload)
{

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	ASC->RemoveLooseGameplayTag(CrouchStartTag);
	ASC->AddLooseGameplayTag(CrouchLoopTag);
	
}


void URS_Couch_Ability::HandleEndRequested(FGameplayEventData Payload)
{

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->AddLooseGameplayTag(CrouchEndTag);

}

void URS_Couch_Ability::OnEndFinished(FGameplayEventData Payload)
{

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

/*
void URS_Couch_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(CrouchStartTag);
		ASC->RemoveLooseGameplayTag(CrouchLoopTag);
		ASC->RemoveLooseGameplayTag(CrouchEndTag);
	}


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

*/

void URS_Couch_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(CrouchStartTag);

		ASC->RemoveLooseGameplayTag(CrouchLoopTag);

		ASC->RemoveLooseGameplayTag(CrouchEndTag);
	}


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}
