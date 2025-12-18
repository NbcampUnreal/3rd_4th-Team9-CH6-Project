// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/RSAnimNotify_CheckHit.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

URSAnimNotify_CheckHit::URSAnimNotify_CheckHit()
	: TriggerGameplayTag()
	, ComboAttackLevel(1.0f)
{
}

FString URSAnimNotify_CheckHit::GetNotifyName_Implementation() const
{
	return TEXT("CheckHit");
}

void URSAnimNotify_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		if (IsValid(OwnerActor) == true && IsValid(AbilitySystemComponent) == true)
		{
			FGameplayEventData PayloadData;
			PayloadData.EventTag = TriggerGameplayTag;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
			PayloadData.EventMagnitude = ComboAttackLevel;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
		}
	}
}
