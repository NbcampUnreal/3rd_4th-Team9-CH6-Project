#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotify/RSAnimEquipAction.h"
#include "RSEquipmentManagerComponent.generated.h"

class URSItemInstance;
class URSItemTemplate;
class URSItemFragment_EquipRequirement;
class URSEquipManagerComponent;
class URSCosmeticManagerComponent;
class UAbilitySystemComponent;
class URSCombatStyleData;
class URSItemFragment_CombatStyle;
class URSHeroData;


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

DECLARE_MULTICAST_DELEGATE_FourParams(
	FOnRSActiveWeaponChanged,
	FGameplayTag,
	FGameplayTag,
	URSItemInstance*,
	URSItemInstance*
);

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnRSEquipAnimAction,
	ERSAnimEquipAction /*Action*/,
	URSItemInstance*   /*PendingItem*/
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
	//UFUNCTION(BlueprintPure, Category = "RS|Equipment")
	//URSItemInstance* GetItemInSlot(const FGameplayTag& SlotTag) const;

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

	URSItemInstance* GetWeaponInSlot(int32 SlotIndex) const;

public:
	// AnimNotify에서 호출되는 진입점(캐릭터->EquipmentManager)
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment|Anim")
	void HandleEquipAnimAction(ERSAnimEquipAction Action);



protected:
	///** 내부 헬퍼: 실제 장착 처리 (검증 완료 후 호출) */
	//void InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem);

	///** 내부 헬퍼: 실제 해제 처리 (검증 완료 후 호출) */
	//void InternalUnequip(const FGameplayTag& SlotTag);

	/** 아이템 템플릿/Fragment 기반으로 슬롯 호환 여부 체크 */
	bool CheckSlotCompatibility(const FGameplayTag& SlotTag, const URSItemTemplate* Template, FText& OutFailReason) const;

	/** EquipRequirement Fragment 기준으로 요구 조건 체크 */
	bool CheckEquipRequirements(const URSItemTemplate* Template, FText& OutFailReason) const;

	///** 해당 슬롯이 “무기 슬롯”인지 여부 (무기 슬롯일 때만 CosmeticManager 호출) */
	//bool IsWeaponSlot(const FGameplayTag& SlotTag) const;


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

	/** 현재 사용 중인 HeroData */
	UPROPERTY()
	TObjectPtr<const URSHeroData> HeroData;





protected:
	/** 무기 슬롯 변화에 따른 “최종 CombatStyle” 결정 */
	URSCombatStyleData* ResolveCombatStyleForWeaponItem(URSItemInstance* WeaponItem) const;

	/** 무기 없을 때 돌아갈 DefaultUnarmedStyle */
	URSCombatStyleData* ResolveDefaultUnarmedStyle() const;


private:
	// YKJ Annotation : 메인 무기 변경 시, 효과/외형을 재구성하는 단일 진입점.
	void HandleMainWeaponChanged(URSItemInstance* OldWeapon, URSItemInstance* NewWeapon);

private:
	/** 중복 적용 방지용 (Old->New 교체가 정확히 1회만 일어나게) */
	UPROPERTY(Transient)
	TWeakObjectPtr<URSCombatStyleData> CachedAppliedStyle;

private:
	// ===== 장착/해제 진행 중 상태 =====
	UPROPERTY(Transient)
	bool bEquipTransactionActive = false;

	UPROPERTY(Transient)
	FGameplayTag PendingSlotTag;

	UPROPERTY(Transient)
	TObjectPtr<URSItemInstance> PendingOldItem = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<URSItemInstance> PendingNewItem = nullptr;

	// 트랜잭션 종료(정리)
	void ClearEquipTransaction();



#pragma region
public:
	// --- Active Weapon API ---
	void SetActiveWeaponSlot(const FGameplayTag& NewActiveSlotTag);

	FGameplayTag GetActiveWeaponSlot() const { return ActiveWeaponSlotTag; }
	URSItemInstance* GetActiveWeaponItem() const;

	UPROPERTY(BlueprintAssignable, Category = "RS|Equipment")
	FRSEquipmentChangedSignature OnEquipmentChanged;

	// 무기 습득: Main/Sub 자동 배치 + 선택적으로 자동 장착
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment|Weapon")
	bool TryPickupWeaponToSlots(URSItemInstance* NewWeaponItem, bool bAutoEquip = true);

	// 슬롯 버튼: 1/2 입력에 대응 (필요하면 스왑 후 Active 변경)
	UFUNCTION(BlueprintCallable, Category = "RS|Equipment|Weapon")
	void RequestActivateWeaponSlot(FGameplayTag RequestedSlot);

	UFUNCTION(BlueprintCallable, Category = "RS|Equipment|Weapon")
	void RequestEquipWeaponByIndex(int32 Index);

public:
	FOnRSActiveWeaponChanged OnActiveWeaponChanged;

	FOnRSEquipAnimAction OnEquipAnimAction;

	URSItemInstance* GetItemInSlot(const FGameplayTag& SlotTag) const;

protected:
	// 기존 구조(예시): EquippedItems / OnEquipmentChanged / InternalEquip / InternalUnequip 등 유지
	// TMap<FGameplayTag, TObjectPtr<URSItemInstance>> EquippedItems;

	bool IsWeaponSlot(const FGameplayTag& SlotTag) const;



	void InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem);
	void InternalUnequip(const FGameplayTag& SlotTag);

private:
	void BroadcastActiveWeaponChanged(
		const FGameplayTag& OldSlot,
		const FGameplayTag& NewSlot,
		URSItemInstance* OldItem,
		URSItemInstance* NewItem
	);

	void SwapWeaponSlots(const FGameplayTag& SlotA, const FGameplayTag& SlotB);
	bool IsWeaponSlotFilled(const FGameplayTag& SlotTag) const;

	void SwapMainAndSubIfNeeded(const FGameplayTag& NewSlot);



private:
	void RebuildWeaponMainSubFromQuickSlots(bool bBroadcast);
	bool SetWeaponQuickSlotItem(int32 Index, URSItemInstance* Item);
	URSItemInstance* GetWeaponQuickSlotItem(int32 Index) const;



private:
	// 1/2 슬롯 네이밍 확정: Slot.Weapon.Main / Slot.Weapon.Sub
	UPROPERTY(Transient)
	FGameplayTag ActiveWeaponSlotTag;

	UPROPERTY(Transient)
	FGameplayTag LastRequestedWeaponSlotTag;


private:
	// UI 1/2 슬롯 SSOT (먹은 순서대로 채워짐, 선택해도 인덱스는 고정)
	UPROPERTY(Transient)
	TObjectPtr<URSItemInstance> WeaponQuickSlots[2] = { nullptr, nullptr };

	// 현재 선택된 QuickSlot 인덱스 (0=N, 1=M)
	UPROPERTY(Transient)
	int32 ActiveQuickSlotIndex = 0;

#pragma endregion

};
