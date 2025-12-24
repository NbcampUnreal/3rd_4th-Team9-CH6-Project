// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/RS_AnimNotify_RollEndCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogRSAbility, Log, All);

void URS_AnimNotify_RollEndCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[AnimNotify] MeshComp is null"));
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[AnimNotify] OwnerActor is null"));
		return;
	}

	if (!EventTag.IsValid())
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[AnimNotify] EventTag is invalid"));
		return;
	}

	UAbilitySystemComponent* ASC = nullptr;

	// AbilitySystemInterface를 통해 ASC 가져오기
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OwnerActor))
	{
		ASC = ASCInterface->GetAbilitySystemComponent();
		FGameplayEventData PayloadData;
		PayloadData.EventTag = EventTag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, PayloadData);
		UE_LOG(LogRSAbility, Log, TEXT("[AnimNotify] Sent event %s to ASC"), *EventTag.ToString());
	}

	if (!ASC)
	{
		UE_LOG(LogRSAbility, Warning, TEXT("[AnimNotify] Owner has no ASC"));
		return;
	}
	
}