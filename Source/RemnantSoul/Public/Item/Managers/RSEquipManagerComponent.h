#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GAS/AS/RSAbilitySet.h"          // FRSAbilitySet_GrantedHandles
#include "Item/RSItemInstance.h"
#include "RSEquipManagerComponent.generated.h"

class UAbilitySystemComponent;
class URSAbilitySet;
class URSCombatStyleData;
class URSInputConfig;
class URSHeroComponent;
class URSItemFragment_AbilitySet;
class URSItemFragment_CombatStyle;

UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSEquipManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSEquipManagerComponent();

	/** 무기/장비 변경 시 외부(EquipmentManager)에서 호출: "이제 이 ItemInstance가 메인 무기다" */
	void OnMainWeaponChanged(URSItemInstance* OldWeapon, URSItemInstance* NewWeapon);

public:
	// EquipmentManager가 장비 변화(Old->New)를 알려주는 진입점
	void HandleEquipmentChanged(const FGameplayTag& SlotTag, URSItemInstance* OldItem, URSItemInstance* NewItem);
	void ApplyCombatStyle(URSCombatStyleData* NewStyle); 
	void ApplyAnimStyleLayers(const URSCombatStyleData* Style);


protected:
	virtual void BeginPlay() override;

private:
	// ===== 핵심: 스타일 트랜잭션 =====
	void ClearCurrentCombatStyle();

	// ===== 스타일 결정부(SSOT) =====
	URSCombatStyleData* ResolveCombatStyleForWeapon(URSItemInstance* WeaponInstance) const;
	URSCombatStyleData* GetDefaultUnarmedStyle() const;

	// ===== GAS Apply helpers =====
	void GiveAbilitySetList(const TArray<TObjectPtr<const URSAbilitySet>>& Sets, TArray<FRSAbilitySet_GrantedHandles>& OutHandles);
	void TakeAbilitySetList(TArray<FRSAbilitySet_GrantedHandles>& InOutHandles);

	// ===== Input overlay helpers =====
	URSHeroComponent* GetHeroComponent() const;
	UAbilitySystemComponent* GetASC() const;

	// ===== Item passive ability set (선택) =====
	void ApplyItemPassiveAbilitySets(URSItemInstance* NewWeapon);
	void ClearItemPassiveAbilitySets();

	// ===== Anim style notify (최소) =====
	void ApplyAnimStyleTags(URSCombatStyleData* Style);
	void ClearAnimStyleTags();

	void ApplyStyleInputOverlay(const URSCombatStyleData* Style);
	void ClearStyleInputOverlay();

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	// 현재 스타일
	UPROPERTY(Transient)
	TObjectPtr<URSCombatStyleData> CurrentStyle = nullptr;

	// 현재 스타일이 부여한 AbilitySet 핸들들
	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> CurrentStyleGrantedHandles;

	//UPROPERTY(Transient)
	//FRSAbilitySet_GrantedHandles CurrentStyleHandles;

	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> ItemPassiveHandles;

	// 현재 무기(메인)
	UPROPERTY(Transient)
	TObjectPtr<URSItemInstance> CurrentMainWeapon = nullptr;

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



private:


};
