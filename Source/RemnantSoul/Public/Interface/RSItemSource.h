#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSItemSource.generated.h"

class URSItemData;

UINTERFACE(Blueprintable)
class URSItemSource : public UInterface
{
	GENERATED_BODY()
};

class REMNANTSOUL_API IRSItemSource
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
	URSItemData* GetItemData() const;
};
