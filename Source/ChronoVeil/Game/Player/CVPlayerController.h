// 1.
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/PlayerController.h"
//#include "CVPlayerController.generated.h"
//
//class ACVPlayerState;
//class UAbilitySystemComponent;
//
///**
// * ChronoVeil 전용 PlayerController
// * - PostProcessInput에서 ASC 입력 처리 전달(태그 기반 Ability 입력)
// */
//UCLASS()
//class CHRONOVEIL_API ACVPlayerController : public APlayerController
//{
//	GENERATED_BODY()
//public:
//	ACVPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	// PlayerController
//	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
//
//	// Helpers
//	UFUNCTION(BlueprintCallable, Category = "CV|Player")
//	ACVPlayerState* GetCVPlayerState() const;
//
//	UFUNCTION(BlueprintCallable, Category = "CV|ASC")
//	UAbilitySystemComponent* GetAbilitySystemComponent() const;
//};



#pragma once

#include "GameFramework/PlayerController.h"
#include "CVPlayerController.generated.h"

class UITAbilitySystemComponent;
class AITPlayerState;

UCLASS()
class CHRONOVEIL_API ACVPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACVPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "CVPlayerController")
	UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "CVPlayerController")
	ACVPlayerState* GetCVPlayerState() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
