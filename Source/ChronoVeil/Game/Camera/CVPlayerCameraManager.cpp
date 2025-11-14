#include "Game/Camera/CVPlayerCameraManager.h"

ACVPlayerCameraManager::ACVPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultFOV = CV_CAMERA_DEFAULT_FOV;
	ViewPitchMin = CV_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = CV_CAMERA_DEFAULT_PITCH_MAX;
}