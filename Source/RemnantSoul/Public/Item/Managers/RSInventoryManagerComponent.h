#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSInventoryManagerComponent.generated.h"

class URSItemInstance;
class URSItemTemplate;

/**
 * RSInventoryManagerComponent
 *
 * - RSCharacter가 가진 "아이템 인스턴스"를 관리하는 인벤토리 매니저
 * - Slot 기반 인벤토리 (1차원 배열)로 시작하고, 나중에 그리드 구조로 확장 가능
 * - 기능:
 *   - AddItem(ByTemplate / ByInstance)
 *   - RemoveItem
 *   - FindItem(ByTemplate / ByTag)
 *   - Slot 접근/조회
 *
 * - 장착/해제, GAS 반영, 코스메틱은 다른 매니저에서 처리
 */
UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSInventoryManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** 인벤토리 전체 슬롯 수 */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	int32 GetMaxSlots() const { return MaxSlots; }

	/** 현재 사용 중인(비어있지 않은) 슬롯 수 */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	int32 GetUsedSlotCount() const;

	/** 특정 슬롯의 아이템 인스턴스 반환 (없으면 nullptr) */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	URSItemInstance* GetItemAtIndex(int32 Index) const;

	/** 슬롯 인덱스가 유효한지 체크 */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	bool IsValidIndex(int32 Index) const;

	/**
	 * 템플릿 기반으로 아이템 추가.
	 * - 내부에서 Template->CreateItemInstance()를 호출해서 인스턴스를 만든다.
	 * - OutAddedCount: 실제로 인벤토리에 들어간 개수 (스택 및 공간에 따라 줄어들 수 있음)
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool AddItemByTemplate(URSItemTemplate* ItemTemplate, int32 Count, int32& OutAddedCount);

	/**
	 * 이미 만들어진 ItemInstance를 인벤토리에 추가.
	 * - 주로 드랍 → 픽업 시, 월드에서 넘겨주는 인스턴스를 그대로 넣고 싶을 때 사용.
	 * - OutAddedCount는 합쳐진 Count를 의미 (스택 분할/병합 시 유용)
	 *
	 * 주의:
	 * - 현재 구현은 "외부 인스턴스를 직접 Slots에 박지 않고", Count를 분해해서
	 *   내부 규칙에 맞는 새 인스턴스로 재분배하는 방식.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool AddItemInstance(URSItemInstance* ItemInstance, int32& OutAddedCount);

	/**
	 * 템플릿 기준으로 아이템 제거 (여러 스택에 걸쳐 있을 수 있음).
	 * - CountToRemove 만큼 제거 시도.
	 * - OutRemovedCount: 실제 제거된 개수.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool RemoveItemByTemplate(URSItemTemplate* ItemTemplate, int32 CountToRemove, int32& OutRemovedCount);

	/**
	 * 특정 인스턴스에서 CountToRemove만큼 제거.
	 * - Count이 0이 되면 슬롯에서 제거.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool RemoveItemInstance(URSItemInstance* ItemInstance, int32 CountToRemove, int32& OutRemovedCount);

	/** 템플릿 기준으로 첫 번째 인스턴스를 찾는다. */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	URSItemInstance* FindFirstItemByTemplate(URSItemTemplate* ItemTemplate) const;

	/** 특정 GameplayTag를 가진 아이템들(템플릿 태그 기준)을 모두 찾는다. */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	void FindItemsWithTag(FGameplayTag Tag, TArray<URSItemInstance*>& OutItems) const;

	/** 인벤토리 전체 슬롯 배열 조회 (BP에서 ForEach 돌리기 용도) */
	const TArray<TObjectPtr<URSItemInstance>>& GetAllSlots() const { return Slots; }

protected:
	/** 실제로 스택 규칙(MaxStackCount)을 적용해서 Count를 분배하는 내부 로직 */
	int32 AddToExistingStacks(URSItemTemplate* Template, int32 Count);
	int32 AddToEmptySlots(URSItemTemplate* Template, int32 Count);

	/** 특정 템플릿의 스택들을 전부 순회하며 Count를 제거하는 내부 로직 */
	int32 RemoveFromStacksByTemplate(URSItemTemplate* Template, int32 CountToRemove);

	/** 인스턴스 하나에서 Count를 제거하고, 0이면 슬롯에서 제거 */
	int32 RemoveFromInstance(URSItemInstance* Instance, int32 CountToRemove);

protected:
	/** 인벤토리 슬롯 개수 (나중에 PawnData나 ClassData에서 가져오는 것으로 확장 가능) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Inventory")
	int32 MaxSlots = 30;

	/** 실제 슬롯 배열 (Index 기반, nullptr = 비어있는 슬롯) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RS|Inventory")
	TArray<TObjectPtr<URSItemInstance>> Slots;
};
