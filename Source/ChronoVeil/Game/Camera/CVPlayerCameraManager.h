#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CVPlayerCameraManager.generated.h"

#define CV_CAMERA_DEFAULT_FOV (80.0f)
#define CV_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define CV_CAMERA_DEFAULT_PITCH_MAX (89.0f)

UCLASS()
class CHRONOVEIL_API ACVPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ACVPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
