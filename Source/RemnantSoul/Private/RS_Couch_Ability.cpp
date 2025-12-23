// Fill out your copyright notice in the Description page of Project Settings.


#include "RS_Couch_Ability.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogRSAbility, Log, All);


URS_Couch_Ability::URS_Couch_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CrouchRootTag  = FGameplayTag::RequestGameplayTag(TEXT("Ability.Crouch"));
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
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Activate"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	check(ASC);

	bEndRequestedDuringStart = false;

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

	//End 요청 대기
	auto* WaitEndRequested =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndRequestedEventTag);
	WaitEndRequested->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnEndRequestedEvent);
	WaitEndRequested->ReadyForActivation();
}

//어빌리티 중복 입력을 방지하기 위함.
void URS_Couch_Ability::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] Input Released"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	// Start 중이면 End 요청을 저장
	if (ASC->HasMatchingGameplayTag(CrouchStartTag))
	{
		UE_LOG(LogRSAbility, Log, TEXT("[Crouch] End requested during Start (Buffered)"));
		bEndRequestedDuringStart = true;
		return;
	}

	// Loop 상태라면 즉시 End 처리
	if (ASC->HasMatchingGameplayTag(CrouchLoopTag))
	{
		HandleEndRequested();
	}
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

	if (bEndRequestedDuringStart)
	{
		bEndRequestedDuringStart = false;
		HandleEndRequested();
	}
}

void URS_Couch_Ability::OnEndRequestedEvent(FGameplayEventData Payload)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] EndRequested"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(CrouchStartTag))
	{
		bEndRequestedDuringStart = true;
		return;
	}

	if (ASC->HasMatchingGameplayTag(CrouchLoopTag))
	{
		HandleEndRequested();
	}
}

void URS_Couch_Ability::HandleEndRequested()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	ASC->RemoveLooseGameplayTag(CrouchLoopTag);
	ASC->AddLooseGameplayTag(CrouchEndTag);

	auto* WaitEndFinished =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EndFinishedEventTag);
	WaitEndFinished->EventReceived.AddDynamic(this, &URS_Couch_Ability::OnEndFinished);
	WaitEndFinished->ReadyForActivation();
}

void URS_Couch_Ability::OnEndFinished(FGameplayEventData Payload)
{
	UE_LOG(LogRSAbility, Log, TEXT("[Crouch] End Finished → Ability End"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

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

	bEndRequestedDuringStart = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}