#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/System/CVGameplayTagStack.h"
#include "CVInventoryItemInstance.generated.h"

class UCVInventoryItemFragment;
class UCVInventoryItemDefinition;

UCLASS()
class CHRONOVEIL_API UCVInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UCVInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = "FragmentClass"))
	const UCVInventoryItemFragment* FindFragmentByClass(TSubclassOf<UCVInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	/** add/remove stack count to stat tag(=gameplay-tag stack) */
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	/** whether stat tag has in StatTags */
	bool HasStatTag(FGameplayTag Tag) const;

	/** get the current count of gameplay-tag stack */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	/** gameplay-tag stacks for inventory item instance */
	UPROPERTY()
	FCVGameplayTagStackContainer StatTags;

	/** Inventory Item의 인스턴스는 어떤 Definition으로부터 만들어졌는지 정보 */
	UPROPERTY()
	TSubclassOf<UCVInventoryItemDefinition> ItemDef;
};
