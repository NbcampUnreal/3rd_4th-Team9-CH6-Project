#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_AbilitySet.generated.h"

class URSAbilitySet;
class UAbilitySystemComponent;
class URSItemInstance;

/**
 * 아이템 장착 시 부여할 AbilitySet 정보를 담는 Fragment
 *
 * - 예)
 *   Sword 템플릿:
 *     - 기본 경공격/강공격 GA 세트
 *     - 검 전용 스킬 GA 세트
 *
 *   Staff 템플릿:
 *     - 기본 마법 발사 GA 세트
 *     - 스태프 전용 차지 스킬 GA 세트
 *
 * - 실제 ASC에 Ability를 부여/제거하는 책임은
 *   RSEquipManagerComponent 쪽에서 담당한다.
 */
USTRUCT(BlueprintType)
struct REMNANTSOUL_API FRSItemAbilitySetEntry
{
	GENERATED_BODY()

	/** 부여할 AbilitySet (Lyra 스타일 URSAbilitySet 가정) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Ability")
	TObjectPtr<URSAbilitySet> AbilitySet = nullptr;

	/**
	 * 이 AbilitySet의 용도/분류 태그 (선택)
	 * - 예: Item.Weapon.Ability.BasicAttack, Item.Weapon.Ability.Skill
	 * - 필요 없으면 비워둬도 됨
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Ability", meta = (Categories = "Item"))
	FGameplayTagContainer AbilityTags;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_AbilitySet : public URSItemFragment
{
	GENERATED_BODY()

public:
	URSItemFragment_AbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 이 아이템이 장착될 때 부여할 AbilitySet 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Ability")
	TArray<FRSItemAbilitySetEntry> AbilitySets;

	/**
	 * 단순 헬퍼: 이 Fragment가 들고 있는 AbilitySet들을 반환
	 * - RSEquipManagerComponent가 ASC에 부여/제거할 때 사용
	 */
	const TArray<FRSItemAbilitySetEntry>& GetAbilitySets() const { return AbilitySets; }

	/** 특정 태그를 가진 AbilitySet만 필터링해서 가져오는 함수 (선택적 유틸) */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Ability")
	void GetAbilitySetsWithTag(FGameplayTag Tag, TArray<URSAbilitySet*>& OutSets) const;

	// 필요하다면 나중에 OnInstanceCreated를 override해서
	// 인스턴스에 Ability 관련 정보를 캐싱할 수도 있다.
};


