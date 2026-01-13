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

    if (!IsValid(MeshComp))
    {
        return;
    }

    AActor* OwnerActor = MeshComp->GetOwner();
    if (!IsValid(OwnerActor))
    {
        return;
    }

    // 멀티/서버 기준(원치 않으면 주석 처리)
    //if (!OwnerActor->HasAuthority())
    //{
    //    return;
    //}
    APawn* PawnOwner = Cast<APawn>(OwnerActor);
    if (PawnOwner && !PawnOwner->IsLocallyControlled())
    {
        return;
    }


    if (!TriggerGameplayTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[AnimNotify_CheckHit] TriggerGameplayTag is INVALID (None). Owner=%s Anim=%s"),
            *GetNameSafe(OwnerActor), *GetNameSafe(Animation));
        return;
    }

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
    if (!IsValid(ASC))
    {
        UE_LOG(LogTemp, Error, TEXT("[AnimNotify_CheckHit] ASC is NULL. Owner=%s"), *GetNameSafe(OwnerActor));
        return;
    }

    FGameplayEventData Payload;
    Payload.EventTag = TriggerGameplayTag;
    Payload.EventMagnitude = ComboAttackLevel;
    Payload.Instigator = OwnerActor;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, Payload);

    UE_LOG(LogTemp, Verbose, TEXT("[AnimNotify_CheckHit] Sent Event=%s Mag=%.2f Owner=%s"),
        *TriggerGameplayTag.ToString(), ComboAttackLevel, *GetNameSafe(OwnerActor));
}

