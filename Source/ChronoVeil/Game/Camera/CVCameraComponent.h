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

//protected:  // 원래는 해당 선언을 해주는게 좋음. 그러나 코드를 수정하는데 어려움이 있어. 주석처리만.. 
	UPROPERTY()
	TObjectPtr<UCVCameraModeStack> CameraModeStack;


	FCVCameraModeDelegate DetermineCameraModeDelegate;

};

