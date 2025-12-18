// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RS_Roll_AbilityTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRollTaskSimpleDelegate);
//멀티플레이 환경이 아님에도 왜 델리게이트 선언을 했는지에 대해서는 AbilityTask가 종료된 것을 Ability에 전달하기 위해서 사용.

UCLASS()
class REMNANTSOUL_API URS_Roll_AbilityTask : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	//Roll_Ability에서 호출할 팩토리 함수
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
	static URS_Roll_AbilityTask* PlayRollMontage(
		UGameplayAbility* OwningAbility,
		UAnimMontage* Montage
	);

	UPROPERTY(BlueprintAssignable)
	FRollTaskSimpleDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FRollTaskSimpleDelegate OnCancelled;


protected:

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

private:

	UPROPERTY()
	UAnimMontage* RollMontage;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
