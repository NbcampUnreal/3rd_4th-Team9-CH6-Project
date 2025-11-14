// CVGameState.cpp
#include "Game/GameModes/CVGameStateBase.h"
#include "Game/GameModes/CVExperienceManagerComponent.h"

ACVGameStateBase::ACVGameStateBase()
{
    ExperienceManagerComponent = CreateDefaultSubobject<UCVExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

