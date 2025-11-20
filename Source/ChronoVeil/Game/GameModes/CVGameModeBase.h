//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/GameModeBase.h"
//#include "CVGameModeBase.generated.h"
//
//class UCVPawnData;
//class UCVExperienceDefinition;
//class UCVExperienceManagerComponent;
//
///**
// * 경량 Lyra 스타일 GameMode
// * - PawnData를 통해 PawnClass를 선택
// * - Spawn 시 PawnExtension에 PawnData 주입
// */
//UCLASS()
//class CHRONOVEIL_API ACVGameModeBase : public AGameModeBase
//{
//	GENERATED_BODY()
//public:
//	ACVGameModeBase();
//
//	// AGameModeBase
//	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
//	virtual void InitGameState() override;
//	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
//	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
//	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
//
//
//public:
//	void HandleMatchAssignmentIfNotExpectingOne();
//	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
//	void OnExperienceLoaded(const UCVExperienceDefinition* CurrentExperience);
//	bool IsExperienceLoaded() const;
//
//	/** 컨트롤러에 할당할 PawnData를 결정 (우선순위: PlayerState → GameMode.DefaultPawnData) */
//	const UCVPawnData* GetPawnDataForController(const AController* InController) const;
//
//protected:
//	/** 경험/매치 시스템이 없을 때 사용할 기본 PawnData (에디터에서 지정) */
//	UPROPERTY(EditDefaultsOnly, Category = "CV|GameMode")
//	TObjectPtr<const UCVPawnData> DefaultPawnData;
//};



#pragma once

#include "GameFramework/GameModeBase.h"
#include "CVGameModeBase.generated.h"

UCLASS()
class CHRONOVEIL_API ACVGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

};
