#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CVCameraMode.generated.h"

class UCVCameraComponent;

struct FCVCameraModeView
{
	FCVCameraModeView();

	void Blend(const FCVCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

UENUM(BlueprintType)
enum class ECVCameraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};


UCLASS(Abstract, NotBlueprintable)
class CHRONOVEIL_API UCVCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UCVCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	UCVCameraComponent* GetCVCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

	FCVCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Calmpmax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	float BlendAlpha;

	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;


	ECVCameraModeBlendFunction BlendFunction;
};


UCLASS()
class UCVCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UCVCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UCVCameraMode* GetCameraModeInstance(TSubclassOf<UCVCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UCVCameraMode>& CameraModeClass);
	void EvaluateStack(float DeltaTime, FCVCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FCVCameraModeView& OutCameraModeView) const;

	UPROPERTY()
	TArray<TObjectPtr<UCVCameraMode>> CameraModeInstances;


	UPROPERTY()
	TArray<TObjectPtr<UCVCameraMode>> CameraModeStack;
};
