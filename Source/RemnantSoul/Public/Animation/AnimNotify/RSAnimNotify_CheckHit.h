// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "RSAnimNotify_CheckHit.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSAnimNotify_CheckHit : public UAnimNotify
{
	GENERATED_BODY()
	

public:
    URSAnimNotify_CheckHit();

protected:
    virtual FString GetNotifyName_Implementation() const override;

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;

protected:
    UPROPERTY(EditAnywhere)
    FGameplayTag TriggerGameplayTag;

    UPROPERTY(EditAnywhere)
    float ComboAttackLevel;

};
