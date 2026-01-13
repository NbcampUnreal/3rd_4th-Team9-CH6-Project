// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSInventoryType.h"
#include "RSInventoryComponent.generated.h"

class URSItemData;

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class REMNANTSOUL_API URSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSInventoryComponent();

	virtual void BeginPlay() override;

	FOnInventoryChanged OnInventoryChanged;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 GetSlotCount() const { return Slots.Num(); }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItem(const URSItemData* ItemData) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(URSItemData* ItemData, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(URSItemData* ItemData, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool UseItem(int32 SlotIndex, AActor* User);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void DebugPrintInventory() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin="1"))
	int32 MaxSlots = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FInventoryItem> Slots;

private:
	int32 FindFirstStackableSlot(const URSItemData* ItemData) const;
	int32 FindFirstEmptySlot() const;
};
