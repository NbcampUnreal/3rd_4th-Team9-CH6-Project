#pragma once

#include "CoreMinimal.h"
#include "Game/Inventory/CVInventoryItemDefinition.h"
#include "CVInventoryFragment_EquippableItem.generated.h"

class UCVEquipmentDefinition;
class UCVInventoryItemInstance;

/**
 * 아이템이 장착 가능한(Equippable) 아이템임을 나타내는 Fragment
 */
UCLASS()
class CHRONOVEIL_API UCVInventoryFragment_EquippableItem : public UCVInventoryItemFragment
{
	GENERATED_BODY()

public:
	/** 이 아이템을 장비로 사용할 때 스폰할 Equipment 정의 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UCVEquipmentDefinition> EquipmentDefinition;

	virtual void OnInstanceCreated(UCVInventoryItemInstance* Instance) const override;
};
