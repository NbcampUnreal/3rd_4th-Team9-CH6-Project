#include "Game/Camera/CVCameraMode.h"
#include "Game/Camera/CVCameraComponent.h"
#include "Game/Camera/CVPlayerCameraManager.h"

FCVCameraModeView::FCVCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(CV_CAMERA_DEFAULT_FOV)
{
}

void FCVCameraModeView::Blend(const FCVCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

UCVCameraMode::UCVCameraMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FieldOfView = CV_CAMERA_DEFAULT_FOV;
	ViewPitchMin = CV_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CV_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = ECVCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UCVCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UCVCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

FVector UCVCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UCVCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UCVCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
	}
	else { BlendAlpha = 1.0f; }

	BlendAlpha = FMath::Clamp(BlendAlpha, 0.0f, 1.0f);

	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case ECVCameraModeBlendFunction::Linear:    BlendWeight = BlendAlpha; break;
	case ECVCameraModeBlendFunction::EaseIn:    BlendWeight = FMath::InterpEaseIn(0.f, 1.f, BlendAlpha, Exponent); break;
	case ECVCameraModeBlendFunction::EaseOut:   BlendWeight = FMath::InterpEaseOut(0.f, 1.f, BlendAlpha, Exponent); break;
	case ECVCameraModeBlendFunction::EaseInOut: BlendWeight = FMath::InterpEaseInOut(0.f, 1.f, BlendAlpha, Exponent); break;
	default: checkNoEntry(); break;
	}

	BlendWeight = FMath::Clamp(BlendWeight, 0.0f, 1.0f);
}

UCVCameraComponent* UCVCameraMode::GetCVCameraComponent() const
{
	return CastChecked<UCVCameraComponent>(GetOuter());
}

AActor* UCVCameraMode::GetTargetActor() const
{
	const UCVCameraComponent* ITCameraComponent = GetCVCameraComponent();
	return ITCameraComponent->GetTargetActor();
}

UCVCameraModeStack::UCVCameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UCVCameraMode* UCVCameraModeStack::GetCameraModeInstance(TSubclassOf<UCVCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	for (UCVCameraMode* CameraMode : CameraModeInstances)
	{
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	UCVCameraMode* NewCameraMode = NewObject<UCVCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UCVCameraModeStack::PushCameraMode(TSubclassOf<UCVCameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	UCVCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;

		CameraMode->BlendAlpha = FMath::Clamp(CameraMode->BlendAlpha, 0.0f, 1.0f);
		CameraMode->BlendWeight = FMath::Clamp(ExistingStackContribution, 0.0f, 1.0f);
	}
	else
	{
		CameraMode->BlendAlpha = 0.0f;
		CameraMode->BlendWeight = 0.0f;
	}

	const bool bShouldBlend = ((CameraMode->BlendTime > 0.f) && (StackSize > 0));
	if (!bShouldBlend)
	{
		CameraMode->BlendAlpha = 1.0f;
		CameraMode->BlendWeight = 1.0f;
	}

	CameraModeStack.Insert(CameraMode, 0);

	if (CameraModeStack.Num() > 0)
	{
		CameraModeStack.Last()->BlendWeight = 1.0f;
	}
}

void UCVCameraModeStack::EvaluateStack(float DeltaTime, FCVCameraModeView& OutCameraModeView)
{
	UpdateStack(DeltaTime);
	BlendStack(OutCameraModeView);
}

void UCVCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UCVCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UCVCameraModeStack::BlendStack(FCVCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	const UCVCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->View;

	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}
