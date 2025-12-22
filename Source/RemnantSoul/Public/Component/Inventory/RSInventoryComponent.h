// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSInventoryType.h"
#include "RSInventoryComponent.generated.h"


UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class REMNANTSOUL_API URSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URSInventoryComponent();
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	const TArray<FInventoryItem>& GetItems() const { return Items; }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItem(const URSItemData* ItemData) const;

	/* === Modify === */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(URSItemData* ItemData, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(URSItemData* ItemData, int32 Count = 1);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TArray<FInventoryItem> Items;

	/* 내부 유틸 */
	FInventoryItem* FindItem(URSItemData* ItemData);

public:	
	
	UFUNCTION(BlueprintCallable)
	void DebugPrintInventory() const;
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
