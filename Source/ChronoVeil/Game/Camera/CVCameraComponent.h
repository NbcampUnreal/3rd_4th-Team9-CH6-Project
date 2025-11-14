// 1.
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Camera/CameraComponent.h"
//#include "Game/Camera/CVCameraMode.h" // UCVCameraMode, UCVCameraModeStack 선언 포함
//#include "CVCameraComponent.generated.h"
//
//class UCVCameraMode;
//class UCVCameraModeStack;
//
///** template forward declaration */
//template <class TClass> class TSubclassOf;
//
///** 현재 CameraMode 클래스를 결정하는 델리게이트 (HeroComponent가 바인딩) */
//DECLARE_DELEGATE_RetVal(TSubclassOf<UCVCameraMode>, FCVCameraModeDelegate);
//
///**
// * CV 전용 카메라 컴포넌트
// * - HeroComponent가 제공하는 DetermineCameraModeDelegate에 따라 매 프레임 CameraModeStack을 갱신
// * - EvaluateStack → View 계산 → PlayerController.ControlRotation 반영 → CameraComponent 상태 적용
// * - TargetActor 오버라이드(드론/차량 시점) 지원
// */
//UCLASS(ClassGroup = (CV), meta = (BlueprintSpawnableComponent))
//class CHRONOVEIL_API UCVCameraComponent : public UCameraComponent
//{
//	GENERATED_BODY()
//public:
//	UCVCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/** Owner Actor에서 카메라 컴포넌트 찾기 유틸 */
//	static UCVCameraComponent* FindCameraComponent(const AActor* Actor)
//	{
//		return (Actor ? Actor->FindComponentByClass<UCVCameraComponent>() : nullptr);
//	}
//
//	/** 외부에서 TargetActor를 임시 교체(드론/스펙테이트 등). nullptr이면 Owner 사용 */
//	UFUNCTION(BlueprintCallable, Category = "CV|Camera")
//	void SetOverrideTargetActor(AActor* InTarget) { OverrideTargetActor = InTarget; }
//
//	UFUNCTION(BlueprintPure, Category = "CV|Camera")

//	AActor* GetTargetActor() const { return (OverrideTargetActor.IsValid() ? OverrideTargetActor.Get() : GetOwner()); }
//
//	/** 현재 CameraMode를 얻는 델리게이트 설정/해제 */
//	void SetDetermineCameraModeDelegate(FCVCameraModeDelegate InDelegate) { DetermineCameraModeDelegate = InDelegate; }
//	void ResetDetermineCameraModeDelegate() { DetermineCameraModeDelegate.Unbind(); }
//
//	/** 외부에서 강제로 스택 업데이트/평가 트리거 (특수 상황 디버깅용) */
//	void ForceEvaluate(float DeltaTime);
//
//	/** 델리게이트가 반환하는 CameraMode를 스택에 반영 */
//	void UpdateCameraModes();
//
//	// UActorComponent
//	virtual void OnRegister() override;
//
//	// UCameraComponent
//	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
//
//private:
//	/** 카메라 모드/블렌딩을 담당하는 스택 */
//	UPROPERTY()
//	TObjectPtr<UCVCameraModeStack> CameraModeStack = nullptr;
//
//	/** 현재 CameraMode를 선택하는 델리게이트 (보통 HeroComponent가 바인딩) */
//	FCVCameraModeDelegate DetermineCameraModeDelegate;
//
//	/** Owner 외의 타깃(드론/차량/관전 대상)을 지원 */
//	UPROPERTY(Transient)
//	TWeakObjectPtr<AActor> OverrideTargetActor;
//};



#pragma once

#include "CoreMinimal.h"
#include "CVCameraMode.h"
#include "Camera/CameraComponent.h"
#include "CVCameraComponent.generated.h"


class UCVCameraModeStack;

template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UCVCameraMode>, FCVCameraModeDelegate);

UCLASS()
class CHRONOVEIL_API UCVCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UCVCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UCVCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCVCameraComponent>() : nullptr); }


	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	UPROPERTY()
	TObjectPtr<UCVCameraModeStack> CameraModeStack;


	FCVCameraModeDelegate DetermineCameraModeDelegate;

};

