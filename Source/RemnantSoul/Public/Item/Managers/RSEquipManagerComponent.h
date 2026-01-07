#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GAS/AS/RSAbilitySet.h"          // FRSAbilitySet_GrantedHandles
#include "Item/RSItemInstance.h"

#include "Item/Managers/RSEquipmentTypes.h"

#include "RSEquipManagerComponent.generated.h"

class UAbilitySystemComponent;
class URSAbilitySet;
class URSCombatStyleData;
class URSInputConfig;
class URSHeroComponent;
class URSItemFragment_AbilitySet;
class URSItemFragment_CombatStyle;
class URSItemInstance;
class URSEquipManagerComponent;
class URSEquipmentManagerComponent;
class URSHeroData;
class ARSCharacter;
class URSCosmeticManagerComponent;

class RSEquipmentTypes;


DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnRSCombatStyleResolved,
	const URSCombatStyleData*
);

USTRUCT()
struct FRSCombatStyleDecision
{
	GENERATED_BODY()

	UPROPERTY()
	bool bShouldChangeStyle = false;

	UPROPERTY()
	const URSCombatStyleData* NewStyle = nullptr;
};

// FRSCombatStyleDecision CalculateCombatStyleFromWeaponSlotInput(int32 SlotIndex);

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSEquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSEquipManagerComponent();

	// --- External API ---
	void InitializeEquipManager(URSHeroComponent* InHeroComponent);

	void EquipWeapon(URSItemInstance* WeaponInstance);
	void UnequipWeapon();

	// --- CombatStyle ---
	const URSCombatStyleData* GetCurrentCombatStyle() const;

	FOnRSCombatStyleResolved OnCombatStyleResolved;

	FGameplayTag ResolveWeaponSlotFromInputTag(FGameplayTag InputTag) const;


public:
	void ApplyAnimStyleLayers(const URSCombatStyleData* Style);
	void ApplyCombatStyle(const URSCombatStyleData* NewStyle);
	void HandleEquipSlotInput(FGameplayTag InputTag);

	//// 외부에서 호출 가능한 단 하나의 진입점
	void EquipWeaponFromSlot(ERSWeaponSlot Slot);

	// 아이템 획득 시 호출
	void OnWeaponPickedUp(class URSItemInstance_Weapon* WeaponInstance);


protected:
	virtual void BeginPlay() override;

	virtual void ApplyUnarmedState();



private:
	// ===== 핵심: 스타일 트랜잭션 =====
	void ClearCurrentCombatStyle();

	// ===== 스타일 결정부(SSOT) =====
	const URSCombatStyleData* ResolveCombatStyleForWeapon(const URSItemInstance* WeaponItem) const;
	const URSCombatStyleData* GetDefaultUnarmedStyle() const;

	// ===== GAS Apply helpers =====
	void GiveAbilitySetList(const TArray<TObjectPtr<const URSAbilitySet>>& Sets, TArray<FRSAbilitySet_GrantedHandles>& OutHandles);
	void TakeAbilitySetList(TArray<FRSAbilitySet_GrantedHandles>& InOutHandles);

	// 아이템(무기) 패시브 AbilitySet 적용
	void ApplyItemPassiveAbilitySets(URSItemInstance* Item);




	// ===== Input overlay helpers =====
	ARSCharacter* GetOwnerCharacter() const;
	URSHeroComponent* GetHeroComponent() const;
	UAbilitySystemComponent* GetASC() const;
	const URSHeroData* GetHeroData() const;

	// ===== Default Style 적용 여부 =====
	bool bDefaultStyleApplied = false;

	void HandleInputReady();
	void ApplyDefaultStyleIfNeeded();

	// ===== Item passive ability set (선택) =====
	//void ApplyItemPassiveAbilitySets(URSItemInstance* NewWeapon);
	void ClearItemPassiveAbilitySets();

	// ===== Anim style notify (최소) =====
	void ApplyAnimStyleTags(const URSCombatStyleData* Style);

	void ClearAnimStyleTags();

	void ApplyStyleInputOverlay(const URSCombatStyleData* Style);
	void ClearStyleInputOverlay();

	// --- Core ---
	const URSCombatStyleData* ResolveDesiredCombatStyle() const;
	void RefreshCombatStyle();


	bool IsWeaponSlot(const FGameplayTag& SlotTag) const;
	bool IsMainWeaponSlot(const FGameplayTag& SlotTag) const;

	void CacheRefs();
	void BindDelegates();

	void HandleActiveWeaponChanged(
		FGameplayTag OldSlot,
		FGameplayTag NewSlot,
		URSItemInstance* OldItem,
		URSItemInstance* NewItem
	);





protected:
	/** 슬롯에 실제 장착된 아이템 관리 */
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;


private:
	// 현재 스타일이 부여한 AbilitySet 핸들들
	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> CurrentStyleGrantedHandles;

	//UPROPERTY(Transient)
	//FRSAbilitySet_GrantedHandles CurrentStyleHandles;

	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> ItemPassiveHandles;

	// 무기 ItemFragment_AbilitySet(패시브)로 부여한 핸들들(선택)
	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> CurrentItemPassiveGrantedHandles;

	// 애니 스타일 태그(ABP가 읽게 하려면 캐릭터/ASC에 전달해야 함. v1은 ASC LooseTags 권장)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Equip|Anim")
	bool bApplyAnimStyleTagsToASC = true;

	// 아이템 AbilitySet을 패시브로만 적용할지(정책)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Equip|Policy")
	bool bApplyItemAbilitySetsAsPassive = true;

	UPROPERTY(Transient)
	FGameplayTagContainer CachedAppliedAnimTags;

	// --- Cached ---
	UPROPERTY()
	URSHeroComponent* HeroComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<const URSCombatStyleData> CurrentCombatStyle = nullptr;

	UPROPERTY(Transient)
	TWeakObjectPtr<URSCosmeticManagerComponent> CachedCosmeticManager;

private:
	UPROPERTY(Transient) TWeakObjectPtr<URSEquipmentManagerComponent> CachedEquipmentManager;
	UPROPERTY(Transient) TWeakObjectPtr<URSCosmeticManagerComponent> CosmeticManagerComp;
	UPROPERTY(Transient) TWeakObjectPtr<URSHeroComponent> CachedHeroComponent;
	UPROPERTY(Transient) TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

private:
	// --- 슬롯 데이터 ---
	TObjectPtr<URSItemInstance_Weapon> MainSlotWeapon;
	TObjectPtr<URSItemInstance_Weapon> SubSlotWeapon;

	// 현재 실제 장착 무기 (SSOT)
	TObjectPtr<URSItemInstance_Weapon> CurrentMainWeapon;

private:
	// --- 내부 파이프라인 ---
	void ClearCurrentWeapon();

	URSItemInstance_Weapon* GetWeaponInSlot(ERSWeaponSlot Slot) const;

	// Slot enum을 GameplayTag로 변환
	FGameplayTag GetSlotTag(ERSWeaponSlot Slot) const;

	void ApplyAbilitySet(URSItemInstance_Weapon* Weapon);
	void ApplyInputConfig(URSItemInstance_Weapon* Weapon);
	void ApplyAnimStyle(URSItemInstance_Weapon* Weapon);
};
