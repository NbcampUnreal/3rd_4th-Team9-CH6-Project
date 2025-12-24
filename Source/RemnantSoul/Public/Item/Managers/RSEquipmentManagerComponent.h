#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSEquipmentManagerComponent.generated.h"

class URSItemInstance;
class URSItemTemplate;
class URSItemFragment_EquipRequirement;
class URSEquipManagerComponent;
class URSCosmeticManagerComponent;
class UAbilitySystemComponent;

/**
 * 슬롯별 장비 상태를 관리하는 “논리 장비 매니저”
 *
 * - SlotTag → URSItemInstance* 매핑 관리
 * - Equip / Unequip / Swap 등 고수준 API 제공
 * - 장착 가능 여부 체크 (SlotTag 호환 + EquipRequirement Fragment)
 * - 실제 스탯/Ability/태그 적용은 RSEquipManagerComponent에게 위임
 * - 무기 외형 변경은 RSCosmeticManagerComponent에게 위임
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FRSEquipmentChangedSignature,
	FGameplayTag, SlotTag,
	URSItemInstance*, OldItem,
	URSItemInstance*, NewItem
);

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSEquipmentManagerComponent();

protected:
	virtual void BeginPlay() override;

	/** Owner의 ASC / EquipManager / CosmeticManager 캐싱 */
	void CacheReferences();

public:
	/**
	 * 지정 슬롯에 장비 장착 시도.
	 * - 성공 시 true, 실패 시 false + OutFailReason 세팅
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment")
	bool EquipItemToSlot(const FGameplayTag& SlotTag, URSItemInstance* ItemInstance, FText& OutFailReason);

	/** 현재 슬롯 장비 해제 */
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment")
	bool UnequipItemFromSlot(const FGameplayTag& SlotTag);

	/** 슬롯에 장착된 아이템 가져오기 */
	UFUNCTION(BlueprintPure, Category = "RS|Equipment")
	URSItemInstance* GetItemInSlot(const FGameplayTag& SlotTag) const;

	/** 해당 슬롯이 존재하는 슬롯인지 여부 */
	UFUNCTION(BlueprintPure, Category = "RS|Equipment")
	bool HasSlot(const FGameplayTag& SlotTag) const;

	/** 아이템이 해당 슬롯에 장착 가능한지 사전 체크 */
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment")
	bool CanEquipItemToSlot(const FGameplayTag& SlotTag, URSItemInstance* ItemInstance, FText& OutFailReason) const;

	/** 모든 슬롯 → 아이템 맵 조회 (BP에서 쓰기 편하게) */
	UFUNCTION(BlueprintPure, Category = "RS|Equipment")
	const TMap<FGameplayTag, URSItemInstance*>& GetAllEquippedItems() const { return EquippedItems; }

	/** MainWeaponSlotTag Getter (ItemManager 등에서 사용) */
	FGameplayTag GetMainWeaponSlotTag() const { return MainWeaponSlotTag; }

public:
	/** 장비 변경 알림 델리게이트 (UI 등에서 바인딩 용) */
	UPROPERTY(BlueprintAssignable, Category = "RS|Equipment")
	FRSEquipmentChangedSignature OnEquipmentChanged;

protected:
	/** 내부 헬퍼: 실제 장착 처리 (검증 완료 후 호출) */
	void InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem);

	/** 내부 헬퍼: 실제 해제 처리 (검증 완료 후 호출) */
	void InternalUnequip(const FGameplayTag& SlotTag);

	/** 아이템 템플릿/Fragment 기반으로 슬롯 호환 여부 체크 */
	bool CheckSlotCompatibility(const FGameplayTag& SlotTag, const URSItemTemplate* Template, FText& OutFailReason) const;

	/** EquipRequirement Fragment 기준으로 요구 조건 체크 */
	bool CheckEquipRequirements(const URSItemTemplate* Template, FText& OutFailReason) const;

	/** 해당 슬롯이 “무기 슬롯”인지 여부 (무기 슬롯일 때만 CosmeticManager 호출) */
	bool IsWeaponSlot(const FGameplayTag& SlotTag) const;


protected:
	/** 이 캐릭터가 사용하는 기본 슬롯 목록 (예: Slot.Weapon.Main, Slot.Armor.Head 등) */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Equipment", meta = (Categories = "Slot"))
	TArray<FGameplayTag> DefaultSlots;

	/** 메인 무기 슬롯 태그 (무기 코스메틱/무기 전투 규칙의 기준 슬롯) */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Equipment", meta = (Categories = "Slot"))
	FGameplayTag MainWeaponSlotTag;

	/** 슬롯 → 장착된 아이템 인스턴스 */
	UPROPERTY(VisibleInstanceOnly, Category = "RS|Equipment")
	TMap<FGameplayTag, URSItemInstance*> EquippedItems;

	/** ASC 캐시 (EquipRequirement에서 Tag/Attribute 확인용) */
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	/** GAS 측 장비 효과 담당 매니저 (스탯/Ability/태그) */
	UPROPERTY(Transient)
	TWeakObjectPtr<URSEquipManagerComponent> CachedEquipManager;

	/** 코스메틱 담당 매니저 (무기 외형 등) */
	UPROPERTY(Transient)
	TWeakObjectPtr<URSCosmeticManagerComponent> CachedCosmeticManager;
};
