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
 * - Slot 기반 인벤토리(1차원 배열)로 시작하고, 나중에 그리드 구조로 확장 가능
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

	/** 슬롯 인덱스가 유효한지 체크 */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	bool IsValidIndex(int32 Index) const;

	/** 특정 슬롯의 아이템 인스턴스 반환 (없으면 nullptr) */
	UFUNCTION(BlueprintPure, Category = "RS|Inventory")
	URSItemInstance* GetItemAtIndex(int32 Index) const;

	/**
	 * 템플릿 기반으로 아이템 추가.
	 * - 내부에서 Template->CreateItemInstance()로 인스턴스를 만들어 슬롯에 넣는다.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool AddItemByTemplate(URSItemTemplate* ItemTemplate, int32 Count, int32& OutAddedCount);

	/**
	 * 외부 ItemInstance의 "Count만" 인벤토리에 흡수한다.
	 * - 인스턴스 자체는 Slots에 들어가지 않는다.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool AbsorbItemInstanceCount(URSItemInstance* ItemInstance, int32& OutAddedCount);

	/**
	 * (하위 호환) 기존 이름 유지용 래퍼.
	 * - 내부적으로 AbsorbItemInstanceCount로 위임한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory", meta = (DeprecatedFunction, DeprecationMessage = "Use AbsorbItemInstanceCount"))
	bool AddItemInstance(URSItemInstance* ItemInstance, int32& OutAddedCount);

	/** 템플릿 기준으로 아이템 제거 (여러 스택에 걸쳐 있을 수 있음). */
	UFUNCTION(BlueprintCallable, Category = "RS|Inventory")
	bool RemoveItemByTemplate(URSItemTemplate* ItemTemplate, int32 CountToRemove, int32& OutRemovedCount);

	/** 특정 인스턴스에서 CountToRemove만큼 제거. Count가 0이면 슬롯에서 제거 */
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

private:
	// YKJ Annotation : 외부에서 직접 슬롯을 만지지 못하게 막고, API로만 조작한다. (팀 협업 안정성)
	int32 AddToExistingStacks(URSItemTemplate* Template, int32 Count);
	int32 AddToEmptySlots(URSItemTemplate* Template, int32 Count);
	int32 RemoveFromStacksByTemplate(URSItemTemplate* Template, int32 CountToRemove);
	int32 RemoveFromInstance(URSItemInstance* Instance, int32 CountToRemove);

private:
	/** 인벤토리 슬롯 개수 (나중에 PawnData/ClassData에서 가져오는 것으로 확장 가능) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxSlots = 30;

	/** 실제 슬롯 배열 (Index 기반, nullptr = 비어있는 슬롯) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RS|Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<URSItemInstance>> Slots;
};
