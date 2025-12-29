#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSCombatStyleData.generated.h"

class URSAbilitySet;
class URSInputConfig;

/**
 * CombatStyleData
 * - 무기/자세(stance) 단위로 적용되는 “전투 스타일” 데이터
 * - EquipManager가 이 데이터를 적용/해제(AbilitySet Handles 관리)
 * - HeroComponent는 OverlayInputConfig를 적용/해제(바인딩 핸들 관리)
 * - AnimBP는 AnimLayerTag(또는 StyleTags)로 레이어/몽타주 분기
 */
UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSCombatStyleData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 디버그/표시용 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle")
	FName StyleId;

	/**
	 * 스타일 적용 시 부여할 AbilitySet들
	 * - 예: Unarmed 기본 공격/회피
	 * - 예: Sword 기본 공격/강공격/가드
	 * - 예: Staff 기본 마법/차지
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle")
	TArray<TObjectPtr<const URSAbilitySet>> AbilitySets;

	/**
	 * 입력 오버레이
	 * - Base IMC/InputAction은 동일하게 유지
	 * - InputAction -> GameplayTag 매핑만 스타일별로 바꾸고 싶을 때 사용
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle")
	TObjectPtr<const URSInputConfig> OverlayInputConfig;

	/**
	 * 애님 레이어/분기 태그
	 * - ABP에서 “현재 스타일 태그”를 보고 레이어 교체
	 * - 예: Anim.CombatStyle.Unarmed / Anim.CombatStyle.Sword / Anim.CombatStyle.Staff
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle", meta = (Categories = "Anim"))
	FGameplayTag AnimLayerTag;

	/** 스타일 자체 분류 태그(선택) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle")
	FGameplayTagContainer StyleTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|CombatStyle|Anim")
	TSubclassOf<UAnimInstance> LinkedAnimLayerClass = nullptr;

	// YKJ Annotation : AnimBP가 읽는 태그. (상태머신/블렌드 조건을 태그로 제어)
	UPROPERTY(EditDefaultsOnly, Category = "RS|CombatStyle|Anim", meta = (Categories = "State"))
	FGameplayTagContainer AnimStyleTags;


};
