#pragma once

#include "Components/ControllerComponent.h"
#include "RSItemManagerComponent.generated.h"

class URSItemInstance;
class ARSPickupableItemBase;
class URSEquipmentManagerComponent;
class URSInventoryManagerComponent;

UCLASS()
class URSItemManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	URSItemManagerComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipment(URSInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, URSEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToInventory(URSEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, URSInventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToInventory(URSInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, URSInventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment(URSEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, URSEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_QuickFromInventory(URSInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_QuickFromEquipment(URSEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromInventory(URSInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipment(URSEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

public:
	/*UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryPickItem(ARSPickupableItemBase* PickupableItemActor);*/

	/*UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryDropItem(URSItemInstance* FromItemInstance, int32 FromItemCount);*/

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure)
	bool IsAllowedComponent(UActorComponent* ActorComponent) const;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	URSInventoryManagerComponent* GetMyInventoryManager() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	URSEquipmentManagerComponent* GetMyEquipmentManager() const;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UActorComponent>> AllowedComponents;
};
