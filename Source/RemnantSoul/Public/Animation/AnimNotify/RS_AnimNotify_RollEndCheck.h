// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "RS_AnimNotify_RollEndCheck.generated.h"

//특정 태그를 Waiting하고 있는 어빌리티들을 대상으로 BP를 여럿 구현하면 클래스를 재사용 가능할 것으로 생각됨.
UCLASS()
class REMNANTSOUL_API URS_AnimNotify_RollEndCheck : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS")
	FGameplayTag EventTag;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
