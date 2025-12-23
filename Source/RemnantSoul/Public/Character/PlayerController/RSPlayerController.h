// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "RSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction; 

UCLASS()
class REMNANTSOUL_API ARSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARSPlayerController();

	void OnPlayerDeath();

protected:
	virtual void BeginPlay() override;
	//virtual void SetupInputComponent() override;

	/* Enhanced Input */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* RollAction;
	
	/* Input Callbacks */
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Attack();
	void Input_Roll();

	
};
