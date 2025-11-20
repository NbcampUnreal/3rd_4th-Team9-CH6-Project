#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CVInventoryItemDefinition.generated.h"

class UCVInventoryItemInstance;

/**
 * 인벤토리 아이템의 Fragment 베이스 클래스
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class CHRONOVEIL_API UCVInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	/** InventoryItemInstance가 생성될 때 호출되는 콜백 */
	virtual void OnInstanceCreated(UCVInventoryItemInstance* Instance) const {}
};

/**
 * 인벤토리 아이템 정의(데이터 에셋용 클래스)
 */
UCLASS()
class CHRONOVEIL_API UCVInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public: // 여기 포인트

	UCVInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Fragment 타입으로 찾기 */
	const UCVInventoryItemFragment* FindFragmentByClass(TSubclassOf<UCVInventoryItemFragment> FragmentClass) const;

	/** Inventory Item 정의(메타) 이름 **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	/** Inventory Item의 Component를 Fragment로 인식하면 된다 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Display")
	TArray<TObjectPtr<UCVInventoryItemFragment>> Fragments;
};
