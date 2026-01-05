// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/RS_ShowVictoryWidgetAbility.h"
#include "IngameUI/RS_VictoryWidget.h"
#include "Blueprint/UserWidget.h"

URS_ShowVictoryWidgetAbility::URS_ShowVictoryWidgetAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("[VictoryAbility] Constructor called"));

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData& TriggerData = AbilityTriggers.AddDefaulted_GetRef();
	TriggerData.TriggerTag =FGameplayTag::RequestGameplayTag(TEXT("Event.UI.Victory"));
	TriggerData.TriggerSource =EGameplayAbilityTriggerSource::GameplayEvent;

	UE_LOG(LogTemp, Warning, TEXT("[VictoryAbility] Trigger registered: %s"),*TriggerData.TriggerTag.ToString());
}


void URS_ShowVictoryWidgetAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	UE_LOG(LogTemp, Error, TEXT("[VictoryAbility] ActivateAbility CALLED"));

	if (TriggerEventData)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[VictoryAbility] TriggerEventTag: %s"),
			*TriggerEventData->EventTag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[VictoryAbility] TriggerEventData is NULL"));
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	VictoryWidget = CreateWidget<URS_VictoryWidget>(GetWorld(), VictoryWidgetClass);
	

	VictoryWidget->OnVictoryWidgetFinished.AddDynamic(this, &URS_ShowVictoryWidgetAbility::OnVictoryWidgetFinished);

	VictoryWidget->AddToViewport();
	UE_LOG(LogTemp, Log, TEXT("[VictoryAbility] Widget added to viewport"));
	
	VictoryWidget->PlayVictorySequence();
	UE_LOG(LogTemp, Log, TEXT("[VictoryAbility] PlayVictorySequence called"));
	
}

void URS_ShowVictoryWidgetAbility::OnVictoryWidgetFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[VictoryAbility] OnVictoryWidgetFinished"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void URS_ShowVictoryWidgetAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{

	UE_LOG(LogTemp, Warning,
	TEXT("[VictoryAbility] EndAbility called (Cancelled=%d)"),
	bWasCancelled);
	
	if (VictoryWidget)
	{
		VictoryWidget->OnVictoryWidgetFinished.RemoveAll(this);
		VictoryWidget->RemoveFromParent();
		VictoryWidget = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
