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

class ACVPlayerState;
class UITAbilitySystemComponent;

UCLASS()
class CHRONOVEIL_API ACVPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACVPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;


	UFUNCTION(BlueprintCallable, Category = "CVPlayerController")
	ACVPlayerState* GetCVPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "CVPlayerController")
	UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const;


protected:
	// 디버깅용: 정말로 EnhancedInputComponent가 생성됐는지 확인 : 언리얼에디터상에서 Hero_DefaultPawn을 Hero_Hannabi로 바꾸면 Crash나는 문제 해결용
	//virtual void SetupInputComponent() override;
};
