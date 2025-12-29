// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/RS_Run_Ability.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "GameplayEffect.h"

URS_Run_Ability::URS_Run_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Run")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Run")));
}

void URS_Run_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	UE_LOG(LogTemp, Warning, TEXT("[Run] ActivateAbility"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC || !RunSpeedGE)
	{
		UE_LOG(LogTemp, Error, TEXT("[Run] Commit Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Run] Commit Success"));

	/* === Run Speed GE 적용 === */
	FGameplayEffectSpecHandle SpecHandle =ASC->MakeOutgoingSpec(RunSpeedGE, 1.f, ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		RunGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		UE_LOG(LogTemp, Warning,
			TEXT("[Run] RunSpeed GE Spec Applied"));
	}

	/* === 종료 이벤트 대기 === */
	WaitEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(TEXT("Event.Run.EndRequested")),
		nullptr,
		false,
		false
	);

	if (WaitEndEventTask)
	{
		WaitEndEventTask->EventReceived.AddDynamic(this,&URS_Run_Ability::OnEndRequested);
		WaitEndEventTask->ReadyForActivation();
	}
}

void URS_Run_Ability::OnEndRequested(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Error, TEXT("[Run] EndRequested Event Received"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;


	// 감속 GE 적용
	FGameplayEffectSpecHandle DecaySpec = ASC->MakeOutgoingSpec(RunSpeedDecayGE, 1.f, ASC->MakeEffectContext());

	if (DecaySpec.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*DecaySpec.Data.Get());

		UE_LOG(LogTemp, Warning,
			TEXT("[Run] RunSpeed Decay GE Applied"));
	}

	DecayGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*DecaySpec.Data.Get());
	UE_LOG(LogTemp, Warning, TEXT("[Run] RunSpeed Decay GE Applied"));
	WaitDecayRemovedTask =UAbilityTask_WaitGameplayEffectRemoved::WaitForGameplayEffectRemoved(this,DecayGEHandle);

	if (WaitDecayRemovedTask)
	{
		WaitDecayRemovedTask->OnRemoved.AddDynamic(
			this,
			&URS_Run_Ability::OnDecayFinished
		);

		WaitDecayRemovedTask->ReadyForActivation();
	}
}

void URS_Run_Ability::OnDecayFinished(
	const FGameplayEffectRemovalInfo& RemovalInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("[Run] Decay GE Finished"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// 가속 GE 제거 (여기서!)
	if (RunGEHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(RunGEHandle);
		RunGEHandle.Invalidate();
	}

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void URS_Run_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UE_LOG(LogTemp, Error, TEXT("[Run] EndAbility Called"));
	

	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}
