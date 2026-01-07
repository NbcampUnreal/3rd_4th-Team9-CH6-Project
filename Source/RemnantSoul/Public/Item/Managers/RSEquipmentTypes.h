// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERSWeaponSlot : uint8
{
	Main UMETA(DisplayName = "Main Slot"),
	Sub  UMETA(DisplayName = "Sub Slot")
};

class REMNANTSOUL_API RSEquipmentTypes
{
public:
	RSEquipmentTypes();
	~RSEquipmentTypes();
};
