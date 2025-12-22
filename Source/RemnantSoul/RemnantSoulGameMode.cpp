// Copyright Epic Games, Inc. All Rights Reserved.

#include "RemnantSoulGameMode.h"
#include "RSGameplayTags.h"

ARemnantSoulGameMode::ARemnantSoulGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_RSCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
