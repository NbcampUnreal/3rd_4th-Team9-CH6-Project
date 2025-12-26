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
	/**
	 * 장비 슬롯 변경 이벤트 처리.
	 *
	 * - SlotTag: 어떤 슬롯인지 (예: Slot.Weapon.Main)
	 * - OldItem: 기존에 장착되어 있던 아이템 (없으면 nullptr)
	 * - NewItem: 새로 장착되는 아이템 (없으면 nullptr)
	 *
	 * RSEquipmentManager에서 이 함수를 호출하게 만들면 된다.
	 */
	GENERATED_BODY()

public:
	URSEquipManagerComponent();

	/** 장비 매니저(RSEquipmentManagerComponent)에서 호출할 진입점 */
	void HandleEquipmentChanged(const FGameplayTag& SlotTag, URSItemInstance* OldItem, URSItemInstance* NewItem);

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

protected:
	/** Owner 캐릭터 캐시 */
	TWeakObjectPtr<ARSCharacter> CachedCharacter;

	/** ASC 캐시 */
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	/** 캐릭터 AttributeSet 캐시 */
	TWeakObjectPtr<URSAttributeSet_Character> CachedCharAttributes;

	/**
	 * 아이템별로 부여된 AbilitySpecHandle 목록
	 * - UPROPERTY 필요 없는 순수 런타임 캐시
	 */
	TMap<TWeakObjectPtr<URSItemInstance>, FRSAbilitySet_GrantedHandles> ItemToGrantedAbilityHandles;
};
