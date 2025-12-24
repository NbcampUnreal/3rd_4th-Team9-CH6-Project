// Fill out your copyright notice in the Description page of Project Settings.


#include "RS_Couch_Ability.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogRSAbility);


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
/*
void URS_Couch_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Activate"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	check(ASC);


	//기존 태그가 남아있을 경우를 대비한 초기화
	ASC->RemoveLooseGameplayTag(CrouchStartTag);
	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->RemoveLooseGameplayTag(CrouchEndTag);

	//Start 진입
	ASC->AddLooseGameplayTag(CrouchStartTag);

	//Start 종료 대기
	auto* WaitStartFinished =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StartFinishedEventTag);
	WaitStartFinished->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnStartFinished);
	WaitStartFinished->ReadyForActivation();

	//Loop 종료 대기
	if (EndRequestedEventTag.IsValid())
	{
		auto* WaitLoopEndRequested =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndRequestedEventTag);

		if (WaitLoopEndRequested)
		{
			WaitLoopEndRequested->EventReceived.AddDynamic(this, &URS_Couch_Ability::HandleEndRequested);
			WaitLoopEndRequested->ReadyForActivation();
		}
	}
	
	WaitEndFinishedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndFinishedEventTag);
	WaitEndFinishedTask->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnEndFinished);
	WaitEndFinishedTask->ReadyForActivation();
}
*/

void URS_Couch_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] ActivateAbility called"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[Crouch] CommitAbility failed, ending ability"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// 기존 태그 초기화
	ASC->RemoveLooseGameplayTag(CrouchStartTag);
	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->RemoveLooseGameplayTag(CrouchEndTag);
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Cleared previous loose tags"));

	// Start 진입
	ASC->AddLooseGameplayTag(CrouchStartTag);
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Added Start Tag: %s"), *CrouchStartTag.ToString());

	// Start 종료 대기
	WaitStartFinished = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StartFinishedEventTag);
	WaitStartFinished->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnStartFinished);
	WaitStartFinished->ReadyForActivation();
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Waiting for EndRequestedEvent: %s"), *StartFinishedEventTag.ToString());


	// Loop 종료 대기
	WaitEndFinishedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndRequestedEventTag);
	WaitEndFinishedTask->EventReceived.AddDynamic(this, &URS_Couch_Ability::HandleEndRequested);
	WaitEndFinishedTask->ReadyForActivation();
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Waiting for EndRequestedEvent: %s"), *EndRequestedEventTag.ToString());

	
	// EndFinished 이벤트 대기
	WaitEndFinishedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndFinishedEventTag);
	WaitEndFinishedTask->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnEndFinished);
	WaitEndFinishedTask->ReadyForActivation();
}

void URS_Couch_Ability::OnStartFinished(FGameplayEventData Payload)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Start → Loop"));

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
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] HandleEndRequested called, EventTag: %s"), *Payload.EventTag.ToString());

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		UE_LOG(LogRSAbility, Error, TEXT("[Crouch] AbilitySystemComponent is nullptr in HandleEndRequested"));
		return;
	}

	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->AddLooseGameplayTag(CrouchEndTag);
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Removed Loop Tag: %s, Added End Tag: %s"), *CrouchLoopTag.ToString(), *CrouchEndTag.ToString());
	UE_LOG(LogRSAbility, Log,
	TEXT("[Crouch] Ability Active in HandleEndRequested: %d"),
	IsActive());
}

void URS_Couch_Ability::OnEndFinished(FGameplayEventData Payload)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] End Finished → Ability End"));

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
	UE_LOG(LogRSAbility, Log, TEXT("[CrouchAbility] EndAbility called. bWasCancelled: %s"), bWasCancelled ? TEXT("true") : TEXT("false"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		UE_LOG(LogRSAbility, Log, TEXT("[CrouchAbility] Removing CrouchStartTag"));
		ASC->RemoveLooseGameplayTag(CrouchStartTag);

		UE_LOG(LogRSAbility, Log, TEXT("[CrouchAbility] Removing CrouchLoopTag"));
		ASC->RemoveLooseGameplayTag(CrouchLoopTag);

		UE_LOG(LogRSAbility, Log, TEXT("[CrouchAbility] Removing CrouchEndTag"));
		ASC->RemoveLooseGameplayTag(CrouchEndTag);
	}
	else
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[CrouchAbility] AbilitySystemComponent is null"));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogRSAbility, Log, TEXT("[CrouchAbility] EndAbility finished"));
}
