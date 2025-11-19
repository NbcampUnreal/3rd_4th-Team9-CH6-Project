#include "Game/Inventory/CVInventoryFragment_EquippableItem.h"
#include "Game/Inventory/CVInventoryItemInstance.h"
#include "Game/Equipment/CVEquipmentDefinition.h"

void UCVInventoryFragment_EquippableItem::OnInstanceCreated(UCVInventoryItemInstance* Instance) const
{
	// 나중에 Instance 생성 시 자동 장비 장착/슬롯 연결 같은 로직을 여기서 처리하면 된다.
}
