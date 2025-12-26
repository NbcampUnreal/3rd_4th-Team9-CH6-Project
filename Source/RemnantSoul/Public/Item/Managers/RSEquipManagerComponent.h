#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GAS/AS/RSAbilitySet.h"
#include "RSEquipManagerComponent.generated.h"

class ARSCharacter;
class UAbilitySystemComponent;
class URSAttributeSet_Character;

class URSItemInstance;
class URSItemTemplate;
class URSItemFragment_EquipStats;
class URSItemFragment_AbilitySet;
class URSItemFragment_WeaponCosmetic;
class URSAbilitySet;
class URSCombatStyleData;
class URSHeroComponent;


/**
 * RSEquipManagerComponent
 *
 * - "장비 변경"에 따른 GAS 측 효과를 담당하는 매니저
 *   - 스탯 보정(AttackRange/AttackDamage 등)
 *   - AbilitySet 부여/제거
 *   - WeaponTypeTag 같은 전투 태그 적용/제거
 *
 * - 논리적 장비 상태(Slot → ItemInstance)는 RSEquipmentManager가 관리
 * - 외형(무기 액터/메시/소켓)은 RSCosmeticManager가 관리
 *
 * - 이 컴포넌트는 오직 ASC/Attribute/Tag/Ability 쪽만 책임진다.
 */

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSEquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSEquipManagerComponent();

	/** 장비 매니저(RSEquipmentManagerComponent)에서 호출할 진입점 */
	void HandleEquipmentChanged(const FGameplayTag& SlotTag, URSItemInstance* OldItem, URSItemInstance* NewItem);

	/** 전투 스타일(Overlay) 적용/해제 */
	void ApplyCombatStyle(URSCombatStyleData* NewStyle);
	void ClearCombatStyle();

protected:
	virtual void BeginPlay() override;

	/** ASC / 캐릭터 / AttributeSet 캐싱 */
	void CacheOwnerAndASC();

	/** 장착/해제 공통 처리 */
	void ApplyEquip(URSItemInstance* ItemInstance, const FGameplayTag& SlotTag);
	void ApplyUnequip(URSItemInstance* ItemInstance, const FGameplayTag& SlotTag);

	/** Fragment 단위 처리 */
	void ApplyEquipStats(const URSItemFragment_EquipStats* EquipStatsFrag, bool bApply);
	void ApplyAbilitySet(URSItemInstance* ItemInstance, const URSItemFragment_AbilitySet* AbilityFrag, bool bApply);
	void ApplyWeaponTypeTag(const URSItemFragment_WeaponCosmetic* WeaponCosFrag, bool bApply);

private:
	UAbilitySystemComponent* GetASC() const;
	URSHeroComponent* GetHero() const;

private:
	/** Owner 캐릭터 캐시 */
	TWeakObjectPtr<ARSCharacter> CachedCharacter;

	/** ASC 캐시 */
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	/** 캐릭터 AttributeSet 캐시 */
	TWeakObjectPtr<URSAttributeSet_Character> CachedCharAttributes;

	/** Hero 캐시(선택) */
	TWeakObjectPtr<URSHeroComponent> CachedHero;

	/** 아이템별로 부여된 AbilitySet 핸들 캐시 */
	TMap<TWeakObjectPtr<URSItemInstance>, FRSAbilitySet_GrantedHandles> ItemToGrantedAbilityHandles;

	/** 현재 전투 스타일(Overlay) */
	UPROPERTY()
	TObjectPtr<URSCombatStyleData> CurrentStyle = nullptr;

	/** 현재 스타일이 부여한 AbilitySet 핸들들 */
	UPROPERTY()
	TArray<FRSAbilitySet_GrantedHandles> StyleGrantedHandles;
};
