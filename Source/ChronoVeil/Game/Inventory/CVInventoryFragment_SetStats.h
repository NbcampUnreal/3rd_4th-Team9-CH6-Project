// Source/ChronoVeil/Game/Inventory/CVInventoryFragment_SetStats.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Game/Inventory/CVInventoryItemDefinition.h"
#include "CVInventoryFragment_SetStats.generated.h"

class UCVInventoryItemInstance;

/**
 * 아이템 인스턴스 생성 시 Stat(GameplayTagStack)을 초기화하는 Fragment
 */
UCLASS()
class CHRONOVEIL_API UCVInventoryFragment_SetStats : public UCVInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UCVInventoryItemInstance* Instance) const override;

	/** InitialItemStats gives constructor's parameters for CVGameplayTagStackContainer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	TMap<FGameplayTag, int32> InitialItemStats;
};
