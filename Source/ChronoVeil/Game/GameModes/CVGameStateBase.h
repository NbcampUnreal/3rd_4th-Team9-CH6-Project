#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CVGameStateBase.generated.h"	

/**
 * 경량 GameState
 * - 필요 시 나중에 Experience/Match 흐름 컴포넌트를 부착
 */
class UCVExperienceManagerComponent;

UCLASS()
class CHRONOVEIL_API ACVGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	ACVGameStateBase();

public:
	UPROPERTY()
	TObjectPtr<UCVExperienceManagerComponent> ExperienceManagerComponent;
};


