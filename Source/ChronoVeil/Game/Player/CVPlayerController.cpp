
// 1.
//#include "Game/Player/CVPlayerController.h"
//#include "Game/Player/CVPlayerState.h"
//#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
//
//ACVPlayerController::ACVPlayerController(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	// CameraManager를 커스텀할 계획이면 여기서 교체:
//	// PlayerCameraManagerClass = ACVPlayerCameraManager::StaticClass();
//}
//
//void ACVPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
//{
//    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
//    {
//        if (UCVAbilitySystemComponent* CVASC = Cast<UCVAbilitySystemComponent>(ASC))
//        {
//            CVASC->ProcessAbilityInput(DeltaTime, bGamePaused);
//        }
//    }
//    Super::PostProcessInput(DeltaTime, bGamePaused);
//}
//
//ACVPlayerState* ACVPlayerController::GetCVPlayerState() const
//{
//	return CastChecked<ACVPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
//}
//
//UAbilitySystemComponent* ACVPlayerController::GetAbilitySystemComponent() const
//{
//	const ACVPlayerState* PS = GetCVPlayerState();
//	return (PS ? PS->GetCVAbilitySystemComponent() : nullptr);
//}







#include "Game/Player/CVPlayerController.h"
#include "Game/Player/CVPlayerState.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
#include "Game/Input/CVEnhancedInputComponent.h"
#include "Game/Camera/CVPlayerCameraManager.h"


ACVPlayerController::ACVPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ACVPlayerCameraManager::StaticClass();
}

void ACVPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

ACVPlayerState* ACVPlayerController::GetCVPlayerState() const
{
	return GetPlayerState<ACVPlayerState>();
}

UCVAbilitySystemComponent* ACVPlayerController::GetCVAbilitySystemComponent() const
{
	ACVPlayerState* CVPlayerState = GetCVPlayerState();
	if (IsValid(CVPlayerState))
	{
		return CVPlayerState->GetCVAbilitySystemComponent();
	}
	return nullptr;
}

void ACVPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	UCVAbilitySystemComponent* CVASC = GetCVAbilitySystemComponent();
	if (IsValid(CVASC))
	{
		CVASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

//void ACVPlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//
//	// 방어 + 디버깅용 로그
//	if (!InputComponent->IsA<UCVEnhancedInputComponent>())
//	{
//		UE_LOG(LogTemp, Warning,
//			TEXT("CVPlayerController::SetupInputComponent - InputComponent is %s (Expected: UCVEnhancedInputComponent)"),
//			*GetNameSafe(InputComponent ? InputComponent->GetClass() : nullptr));
//	}
//}