#pragma once

#include "CoreMinimal.h"
#include "RSAnimEquipAction.generated.h"

UENUM(BlueprintType)
enum class ERSAnimEquipAction : uint8
{
	AttachWeapon     UMETA(DisplayName = "AttachWeapon"),
	DetachWeapon     UMETA(DisplayName = "DetachWeapon"),
	ApplyStyle       UMETA(DisplayName = "ApplyStyle"),
	ClearStyle       UMETA(DisplayName = "ClearStyle"),
	EnableCombat     UMETA(DisplayName = "EnableCombat"),
	DisableCombat    UMETA(DisplayName = "DisableCombat"),
};
