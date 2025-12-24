// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "RS_Couch_Ability.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class REMNANTSOUL_API URS_Couch_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URS_Couch_Ability();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled);
	

	//어빌리티를 호출하기 위한 코드
	UPROPERTY(EditDefaultsOnly, Category="Crouch|Tags")
	FGameplayTag CrouchRootTag;

	UPROPERTY(EditDefaultsOnly, Category="Crouch|Tags")
	FGameplayTag CrouchStartTag;

	UPROPERTY(EditDefaultsOnly, Category="Crouch|Tags")
	FGameplayTag CrouchLoopTag;

	UPROPERTY(EditDefaultsOnly, Category="Crouch|Tags")
	FGameplayTag CrouchEndTag;


	//시작, 종료 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category="Crouch|Events")
	FGameplayTag StartFinishedEventTag;

	UPROPERTY(EditDefaultsOnly, Category="Crouch|Events")
	FGameplayTag EndFinishedEventTag;
	
	UPROPERTY(EditDefaultsOnly, Category="Crouch|Events")
	FGameplayTag EndRequestedEventTag;

	UFUNCTION()
	void OnStartFinished(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnEndFinished(FGameplayEventData Payload);

	UFUNCTION()
	void HandleEndRequested(FGameplayEventData Payload);
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEndFinishedTask;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitLoopEndRequested;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitStartFinished;
};
