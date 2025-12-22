// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSInventoryType.generated.h"

/**
 * 
 */
 class URSItemData;
 
 USTRUCT(BlueprintType)
 struct FInventoryItem
 {
 	GENERATED_BODY()
 
 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 	TObjectPtr<URSItemData> ItemData = nullptr;
 
 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 	int32 Count = 0;
 };
 
class REMNANTSOUL_API RSInventoryType
{
public:
	
};
