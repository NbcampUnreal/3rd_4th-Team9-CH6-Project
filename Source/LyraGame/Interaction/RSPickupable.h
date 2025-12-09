#pragma once

#include "RSDefine.h"
#include "UObject/Interface.h"
#include "RSPickupable.generated.h"

class URSItemTemplate;
class URSItemInstance;

USTRUCT(BlueprintType)
struct FRSPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<URSItemTemplate> ItemTemplateClass;

	UPROPERTY(EditAnywhere)
	EItemRarity ItemRarity = EItemRarity::Poor;
	
	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FRSPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<URSItemInstance> ItemInstance;

	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FRSPickupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRSPickupTemplate PickupTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRSPickupInstance PickupInstance;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class URSPickupable : public UInterface
{
	GENERATED_BODY()
};

class IRSPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FRSPickupInfo GetPickupInfo() const = 0;
};
