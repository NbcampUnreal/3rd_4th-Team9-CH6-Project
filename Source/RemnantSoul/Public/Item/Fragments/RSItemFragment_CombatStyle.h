#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_CombatStyle.generated.h"

class URSCombatStyleData;
class UDataTable;

/**
 * 무기(또는 장비)가 유발하는 CombatStyle 선언 Fragment
 *
 * 책임:
 * - "이 아이템을 메인 무기로 들었을 때 어떤 스타일을 적용할지"에 대한 데이터 제공
 * - 실제 적용(AbilitySet/InputOverlay/AnimStyleTags)은 URSEquipManagerComponent가 수행
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_CombatStyle : public URSItemFragment
{
	GENERATED_BODY()

public:
	/** 1순위: 명시적으로 지정한 CombatStyle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Style")
	TObjectPtr<URSCombatStyleData> CombatStyle = nullptr;

	/**
	 * 2순위: CombatStyle이 null일 때, WeaponTypeTag 같은 태그로 스타일을 찾는 Fallback
	 * - 예: Weapon.Sword, Weapon.Staff
	 * - 무기가 아닌 장비는 비워도 됨
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Style", meta = (Categories = "Weapon"))
	FGameplayTag StyleKeyTag;

	/**
	 * 3순위(선택): StyleKeyTag로 매핑할 테이블/에셋(팀 규칙에 따라 DataTable or MapAsset)
	 * - v1에서 꼭 필요 없으면 비워도 됨 (CombatStyle만으로도 충분)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Style")
	TObjectPtr<UDataTable> StyleResolverTable = nullptr;

	/** 이 스타일이 적용되는 범위: 지금은 "메인 무기"만 사용하도록 고정(확장 대비) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Style")
	bool bAffectsMainWeaponOnly = true;

public:
	/** EquipManager가 호출: 이 아이템이 유발하는 최종 CombatStyle을 결정 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Style")
	URSCombatStyleData* ResolveCombatStyle() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
