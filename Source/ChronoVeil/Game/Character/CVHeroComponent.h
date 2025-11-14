// 1.
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/PawnComponent.h"
//#include "GameplayTagContainer.h"                 // FGameplayTag 쓰면 필수
//#include "Game/Input/CVMappableConfigPair.h"      // FCVMappableConfigPair 정의
//#include "CVHeroComponent.generated.h"
//
//class ACVCharacter;
//class UCVPawnData;
//class UCVPawnExtensionComponent;
//class UInputComponent;
//class UCVInputConfig;
//class UEnhancedInputLocalPlayerSubsystem;
////class UCVCameraComponent;      // 카메라 컴포넌트는 나중에 적용시킬 예정임.
//class UCVCameraMode;
//
//class UCVAbilitySystemComponent;
//
//struct FInputActionValue;
////struct FCVMappableConfigPair;
////struct FCVGameplayTags;
//
//USTRUCT()
//struct FCVLayerEntry
//{
//	GENERATED_BODY()
//
//	UPROPERTY() TArray<TObjectPtr<class UInputMappingContext>> Contexts;
//	UPROPERTY() TArray<int32> Priorities;
//	int32 RefCount = 0;
//};
//
///**
// * 입력/카메라 초기화 + IMC Push/Pop 스위칭의 허브
// * - ACVCharacter에서 Possessed/OnRep_Controller 때 호출
// */
//UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
//class CHRONOVEIL_API UCVHeroComponent : public UPawnComponent
//{
//	GENERATED_BODY()
//
//public:
//	UCVHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/** 서버/클라 공통: 소유 직후 1차 초기화 훅 */
//	void HandlePossessed(ACVCharacter* Character);
//
//	/** 클라: Controller 복제 반영(로컬 입력/카메라 재바인딩) */
//	void HandleControllerChanged();
//
//	/** 로컬 입력 바인딩(EnhancedInput) */
//	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
//
//	/** 카메라 모드 결정(카메라 스택 사용 시) */
//	TSubclassOf<UCVCameraMode> DetermineCameraMode() const;
//
//	// ─── 입력 핸들러 예시 ────────────────────────────────────
//	void Input_Move(const FInputActionValue& ActionValue);
//	void Input_LookMouse(const FInputActionValue& ActionValue);
//	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
//	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
//
//public:
//
//	// 태그 - 드리븐 레이어 연동 API
//	void RegisterTarDrivneLayers();
//	void UnregisterTagDrivenLayers();
//	void PushInputLayerByTag(const FGameplayTag& LayerTag);
//	void PopInputLayerByTag(const FGameplayTag& LayerTag);
//
//
//private:
//	/** 내부 유틸: LocalPlayer Subsystem 얻기 */
//	UEnhancedInputLocalPlayerSubsystem* GetLocalPlayerSubsystem() const;
//
//	/** PawnData 캐시 접근 */
//	const UCVPawnData* GetPawnDataSafe() const;
//
//	/** InputConfig 캐시 접근 */
//	const UCVInputConfig* GetInputConfigSafe() const;
//
//private:
//	/** 기본으로 활성화할 매핑 컨텍스트들(네 DataAsset 타입 사용 시 교체) */
//	UPROPERTY(EditAnywhere, Category = "CV|Input")
//	TArray<FCVMappableConfigPair> DefaultInputConfigs; // TODO: 네가 쓰는 페어 구조에 맞춰 타입 교체
//};
//
//



#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CVHeroComponent.generated.h"

class ACVCharacter;
class UCVCameraMode;
struct FInputActionValue;

/**
 * Pawn 또는 Character의 입력과 카메라 관련 기능을 담당하는 Component
 * ULyraHeroComponent을 참고하여 구현함.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CHRONOVEIL_API UCVHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCVHeroComponent(const FObjectInitializer& ObjectInitializer);
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	TSubclassOf<UCVCameraMode> DetermineCameraMode() const;
	void TryBindCameraMode();


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_Aim(const FInputActionValue& InputActionValue);

public:
	void OnAimStart(const FInputActionValue& Value);
	void OnAimEnd(const FInputActionValue& Value);

public:
	UPROPERTY(Transient)
	bool bIsAiming = false;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCVCameraMode> AimCameraModeClass;

private:
	FORCEINLINE ACVCharacter* GetOwnerCharacter();
	FORCEINLINE const ACVCharacter* GetOwnerCharacter() const;
};
