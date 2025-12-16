// Copyright Epic Games, Inc. All Rights Reserved.


#include "RemnantSoulPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "RemnantSoul.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ARemnantSoulPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogRemnantSoul, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

//void ARemnantSoulPlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//
//	// only add IMCs for local player controllers
//	if (IsLocalPlayerController())
//	{
//		// Add Input Mapping Contexts
//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
//		{
//			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
//			{
//				Subsystem->AddMappingContext(CurrentContext, 0);
//			}
//
//			// only add these IMCs if we're not using mobile touch input
//			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
//			{
//				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
//				{
//					Subsystem->AddMappingContext(CurrentContext, 0);
//				}
//			}
//		}
//	}
//}

void ARemnantSoulPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    const bool bTouch = SVirtualJoystick::ShouldDisplayTouchInterface();
    UE_LOG(LogTemp, Warning, TEXT("[PC] ShouldDisplayTouchInterface=%d  IsLocalPlayerController=%d"),
        bTouch ? 1 : 0, IsLocalPlayerController() ? 1 : 0);

    if (IsLocalPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            UE_LOG(LogTemp, Warning, TEXT("[PC] DefaultMappingContexts Num=%d"), DefaultMappingContexts.Num());
            for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            {
                UE_LOG(LogTemp, Warning, TEXT("[PC] Add Default IMC: %s"),
                    CurrentContext ? *CurrentContext->GetName() : TEXT("NULL"));
                Subsystem->AddMappingContext(CurrentContext, 0);
            }

            UE_LOG(LogTemp, Warning, TEXT("[PC] MobileExcludedMappingContexts Num=%d"), MobileExcludedMappingContexts.Num());
            if (!bTouch)
            {
                for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[PC] Add MobileExcluded IMC: %s"),
                        CurrentContext ? *CurrentContext->GetName() : TEXT("NULL"));
                    Subsystem->AddMappingContext(CurrentContext, 0);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[PC] TouchInterface ON -> MobileExcluded IMCs NOT added"));
            }
        }
    }
}
