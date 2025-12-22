// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "RSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	/* === Roll === */

	// Roll 방향 각도 (-180 ~ +180)
	UPROPERTY(BlueprintReadOnly, Category = "Roll")
	float RollAngle = 0.f;

	// 현재 Roll 상태 여부
	UPROPERTY(BlueprintReadOnly, Category = "Roll")
	bool bIsRolling = false;

public:

	UFUNCTION(BlueprintCallable)
	void SetRollAngle(float InAngle);

	UFUNCTION(BlueprintCallable)
	void SetIsRolling(bool bInRolling);
};
