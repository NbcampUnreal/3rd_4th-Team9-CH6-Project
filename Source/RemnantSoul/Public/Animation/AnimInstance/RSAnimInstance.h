// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "RSAnimInstance.generated.h"

class UAbilitySystemComponent;

/**
 * RS Anim Instance
 * - Roll : Ability → 직접 값 전달
 * - Crouch : GameplayTag 기반 상태 조회
 */
UCLASS()
class REMNANTSOUL_API URSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	/* === UAnimInstance Overrides === */

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/* === Roll === */

	// Roll 방향 각도 (-180 ~ +180)
	UPROPERTY(BlueprintReadOnly, Category = "Roll")
	float RollAngle = 0.f;

	// 현재 Roll 상태 여부
	UPROPERTY(BlueprintReadOnly, Category = "Roll")
	bool bIsRolling = false;

	UFUNCTION(BlueprintCallable)
	void SetRollAngle(float InAngle);

	UFUNCTION(BlueprintCallable)
	void SetIsRolling(bool bInRolling);

	/* === Crouch === */

	UPROPERTY(BlueprintReadOnly, Category = "Crouch")
	bool bCrouchStart = false;

	UPROPERTY(BlueprintReadOnly, Category = "Crouch")
	bool bCrouchLoop = false;

	UPROPERTY(BlueprintReadOnly, Category = "Crouch")
	bool bCrouchEnd = false;

	
protected:

	/* === GAS === */

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/* === Cached Gameplay Tags === */

	FGameplayTag CrouchStartTag;
	FGameplayTag CrouchLoopTag;
	FGameplayTag CrouchEndTag;

	UPROPERTY(BlueprintReadOnly, Category="Crouch")
	bool bIsCrouching = false;

};