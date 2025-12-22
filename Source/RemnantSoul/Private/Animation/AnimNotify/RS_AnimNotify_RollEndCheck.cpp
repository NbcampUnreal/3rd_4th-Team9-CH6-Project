// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/RS_AnimNotify_RollEndCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Actor.h"

void URS_AnimNotify_RollEndCheck::Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !EventTag.IsValid())
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = OwnerActor;
	EventData.Target = OwnerActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor,EventTag,EventData);
}