// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerController/RSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/RSBaseCharacter.h"

ARSPlayerController::ARSPlayerController()
{
	bShowMouseCursor = false;
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/* Mapping Context 등록 */
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ARSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(InputComponent);

	if (!EnhancedInput)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ARSPlayerController::Input_Move);
	}

	if (LookAction)
	{
		EnhancedInput->BindAction(LookAction,ETriggerEvent::Triggered,this,&ARSPlayerController::Input_Look
		);
	}

	if (AttackAction)
	{
		EnhancedInput->BindAction(AttackAction,ETriggerEvent::Started,this,&ARSPlayerController::Input_Attack
		);
	}
}

/* ================= Input ================= */

void ARSPlayerController::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	if (ARSBaseCharacter* Ch = Cast<ARSBaseCharacter>(GetPawn()))
	{
		Ch->HandleMoveInput(MoveVector);
	}
}

void ARSPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (ARSBaseCharacter* Ch = Cast<ARSBaseCharacter>(GetPawn()))
	{
		Ch->HandleLookInput(LookVector);
	}
}

void ARSPlayerController::Input_Attack()
{
	if (ARSBaseCharacter* Ch = Cast<ARSBaseCharacter>(GetPawn()))
	{
		Ch->HandleAttackInput();
	}
}

void ARSPlayerController::OnPlayerDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] Player Death UI"));

	// 마우스 커서 표시
	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);

	/* 추후 사망 위젯 작업에 따라 알맞게 수정 필요함.
	if (DeathWidgetClass && !DeathWidgetInstance)
	{
		DeathWidgetInstance = CreateWidget<UUserWidget>(
			this,
			DeathWidgetClass
		);

		if (DeathWidgetInstance)
		{
			DeathWidgetInstance->AddToViewport(100);
		}
	}
	*/
}