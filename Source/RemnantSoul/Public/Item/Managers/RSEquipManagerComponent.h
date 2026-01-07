#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GAS/AS/RSAbilitySet.h"
#include "Item/Managers/RSEquipmentTypes.h"
#include "RSEquipManagerComponent.generated.h"

#ifndef RS_LEGACY_ENUM_SLOT_PIPELINE
#define RS_LEGACY_ENUM_SLOT_PIPELINE 1
#endif

class ARSCharacter;
class UAbilitySystemComponent;
class URSCombatStyleData;
class URSHeroComponent;
class URSHeroData;
class URSItemInstance;
class URSItemInstance_Weapon;
class URSEquipmentManagerComponent;
class URSCosmeticManagerComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRSCombatStyleResolved, const URSCombatStyleData*);

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSEquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSEquipManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	/* 외부 입력(EnhancedInput Tag) 진입점 */
	void HandleEquipSlotInput(FGameplayTag InputTag);

	/* SSOT 진입점: 슬롯 태그로 장착 요청 (EquipmentManager에게 위임) */
	UFUNCTION(BlueprintCallable, Category = "RS|Equip|Weapon")
	void EquipWeaponFromSlotTag(FGameplayTag SlotTag);

	/* InputTag -> SlotTag 유틸 (CPP에서 사용 중) */
	FGameplayTag ResolveWeaponSlotFromInputTag(FGameplayTag InputTag) const;

	/* CombatStyle resolve 이벤트 (CPP에서 Broadcast 사용 중) */
	FOnRSCombatStyleResolved OnCombatStyleResolved;

	/* (레거시/호환용) Enum 기반 진입점: 내부 적용 금지, Tag 파이프라인으로 라우팅 */
	UFUNCTION(BlueprintCallable, Category = "RS|Equip|Weapon|Legacy", meta = (DeprecatedFunction, DeprecationMessage = "Use EquipWeaponFromSlotTag"))
	void EquipWeaponFromSlot(ERSWeaponSlot Slot);

	/* (레거시/호환용) 픽업 라우팅 */
	UFUNCTION(BlueprintCallable, Category = "RS|Equip|Weapon|Legacy")
	void OnWeaponPickedUp(URSItemInstance* WeaponInstance);

private:
	// ===== refs / delegates =====
	void CacheRefs();
	void BindDelegates();

	// ===== core style transaction =====
	void ClearCurrentCombatStyle();
	void ApplyCombatStyle(const URSCombatStyleData* NewStyle);
	const URSCombatStyleData* ResolveCombatStyleForWeapon(const URSItemInstance* WeaponItem) const;
	const URSCombatStyleData* GetDefaultUnarmedStyle() const;

	// ===== GAS helpers =====
	UAbilitySystemComponent* GetASC() const;
	void GiveAbilitySetList(const TArray<TObjectPtr<const URSAbilitySet>>& Sets, TArray<FRSAbilitySet_GrantedHandles>& OutHandles);
	void TakeAbilitySetList(TArray<FRSAbilitySet_GrantedHandles>& InOutHandles);
	void ApplyItemPassiveAbilitySets(URSItemInstance* Item);
	void ClearItemPassiveAbilitySets();

	// ===== input overlay / anim tags =====
	URSHeroComponent* GetHeroComponent() const;
	const URSHeroData* GetHeroData() const;
	void ApplyStyleInputOverlay(const URSCombatStyleData* Style);
	void ClearStyleInputOverlay();
	void ApplyAnimStyleTags(const URSCombatStyleData* Style);
	void ClearAnimStyleTags();
	void ApplyAnimStyleLayers(const URSCombatStyleData* Style);

	// ===== default style gate =====
	void ApplyDefaultStyleIfNeeded();

	// ===== slots =====
	bool IsWeaponSlot(const FGameplayTag& SlotTag) const;
	bool IsMainWeaponSlot(const FGameplayTag& SlotTag) const;

	// ===== event handlers =====
	void HandleActiveWeaponChanged(FGameplayTag OldSlot, FGameplayTag NewSlot, URSItemInstance* OldItem, URSItemInstance* NewItem);

	// ===== utility =====
	void HandleInputReady();
	ARSCharacter* GetOwnerCharacter() const;

private:
	// Cached refs
	UPROPERTY(Transient) TWeakObjectPtr<URSEquipmentManagerComponent> CachedEquipmentManager;
	UPROPERTY(Transient) TWeakObjectPtr<URSCosmeticManagerComponent> CachedCosmeticManager;
	UPROPERTY(Transient) TWeakObjectPtr<URSHeroComponent> CachedHeroComponent;
	UPROPERTY(Transient) TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	// Style state
	UPROPERTY(Transient) TObjectPtr<const URSCombatStyleData> CurrentCombatStyle = nullptr;
	UPROPERTY(Transient) TArray<FRSAbilitySet_GrantedHandles> CurrentStyleGrantedHandles;

	// Item passive handles (CPP에서 사용 중: ItemPassiveHandles)
	UPROPERTY(Transient) TArray<FRSAbilitySet_GrantedHandles> ItemPassiveHandles;

	// AnimStyleTags 캐시 (CPP에서 CachedAppliedAnimTags 사용 중)
	UPROPERTY(Transient) FGameplayTagContainer CachedAppliedAnimTags;

	// Anim notify cosmetic 추적 (CPP에서 LastCosmeticAppliedItem 사용 중)
	UPROPERTY(Transient) TWeakObjectPtr<URSItemInstance> LastCosmeticAppliedItem;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Equip|Anim")
	bool bApplyAnimStyleTagsToASC = true;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Equip|Policy")
	bool bApplyItemAbilitySetsAsPassive = true;

	bool bDelegatesBound = false;
	bool bDefaultStyleApplied = false;
};
