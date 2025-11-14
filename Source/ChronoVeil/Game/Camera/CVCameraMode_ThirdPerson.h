#pragma once

#include "CoreMinimal.h"
#include "Game/Camera/CVCameraMode.h"
#include "CVCameraMode_ThirdPerson.generated.h"

class UCurveVector;

UCLASS(Abstract, Blueprintable)
class CHRONOVEIL_API UCVCameraMode_ThirdPerson : public UCVCameraMode
{
	GENERATED_BODY()

public:
	UCVCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void UpdateView(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};
