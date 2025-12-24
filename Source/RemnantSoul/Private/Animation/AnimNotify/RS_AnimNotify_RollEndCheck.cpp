// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/RS_AnimNotify_RollEndCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"


void URS_AnimNotify_RollEndCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (!EventTag.IsValid())
	{
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
	}

	if (!ASC)
	{
		return;
	}
	
}