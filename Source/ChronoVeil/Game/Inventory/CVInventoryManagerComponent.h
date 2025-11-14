#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CVInventoryManagerComponent.generated.h"

class UCVInventoryItemInstance;
class UCVInventoryItemDefinition;

USTRUCT(BlueprintType)
struct FCVInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UCVInventoryItemInstance> Instance = nullptr;
};

USTRUCT(BlueprintType)
struct FCVInventoryList
{
	GENERATED_BODY()

	FCVInventoryList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{
	}

	UCVInventoryItemInstance* AddEntry(TSubclassOf<UCVInventoryItemDefinition> ItemDef);

	UPROPERTY()
	TArray<FCVInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CHRONOVEIL_API UCVInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCVInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** InventoryItemDefinition을 통해 인스턴스를 생성하고 리스트에 추가 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UCVInventoryItemInstance* AddItemDefinition(TSubclassOf<UCVInventoryItemDefinition> ItemDef);

	UPROPERTY()
	FCVInventoryList InventoryList;
};
