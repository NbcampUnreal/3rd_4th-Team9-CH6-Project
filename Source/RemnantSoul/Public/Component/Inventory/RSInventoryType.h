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
 	TObjectPtr<URSItemData> ItemData;

 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 	int32 Count = 0;

 	bool IsEmpty() const
 	{
 		return ItemData == nullptr || Count <= 0;
 	}

 	void Clear()
 	{
 		ItemData = nullptr;
 		Count = 0;
 	}
 };
 
class REMNANTSOUL_API RSInventoryType
{
public:
	
};
