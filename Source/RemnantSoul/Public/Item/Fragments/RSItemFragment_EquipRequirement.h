#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_EquipRequirement.generated.h"

class URSItemInstance;

/**
 * 아이템 장착 요구 조건을 나타내는 Fragment
 *
 * - 예시 조건:
 *   - MinCharacterLevel 이상이어야 함
 *   - 특정 Class 태그를 가져야 함 (예: Character.Class.Swordman)
 *   - 특정 상태 태그를 가지고 있으면 장착 불가 (예: State.IsDead)
 *
 * - 실제 조건 체크는 보통 RSEquipmentManagerComponent에서 수행하고,
 *   이 Fragment는 "데이터 + 헬퍼" 역할만 한다.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_EquipRequirement : public URSItemFragment
{
	GENERATED_BODY()

public:
	URSItemFragment_EquipRequirement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/** 요구 최소 레벨 (0이면 무시) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (ClampMin = "0"))
	int32 MinCharacterLevel = 0;

	/** 요구 최대 레벨 (0이면 무시, 또는 Min보다 작으면 무시) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (ClampMin = "0"))
	int32 MaxCharacterLevel = 0;

	/**
	 * 소유자가 반드시 가지고 있어야 하는 태그들
	 * - 예: Character.Class.Swordman, Character.Stat.StrHigh
	 * - 보통은 RSGameplayTags에 Native Tag로 추가한 것을 여기서 사용
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (Categories = ""))
	FGameplayTagContainer RequiredOwnerTags;

	/**
	 * 소유자가 가지고 있으면 안 되는 태그들
	 * - 예: State.IsDead, State.CannotEquipWeapon
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (Categories = ""))
	FGameplayTagContainer BlockedOwnerTags;

	/**
	 * 이 아이템(템플릿)이 반드시 가져야 하는 태그들
	 * - 예: Item.Weapon.SwordOnly, Item.Class.MageOnly 등
	 * - 템플릿의 ItemTags와 비교해서 검증할 수 있다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (Categories = "Item"))
	FGameplayTagContainer RequiredItemTags;

	/**
	 * 이 아이템(템플릿)이 가져서는 안 되는 태그들
	 * - 잘 쓰진 않겠지만, 전용 규칙이 필요하다면 확장 가능
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipRequirement", meta = (Categories = "Item"))
	FGameplayTagContainer BlockedItemTags;

public:
	/**
	 * 장착 요구 조건을 체크하는 헬퍼 함수
	 *
	 * @param OwnerTags         : 소유자(캐릭터/컨트롤러/ASC)가 가진 GameplayTagContainer
	 * @param ItemTags          : 아이템 템플릿의 ItemTags
	 * @param CharacterLevel    : 소유자의 레벨 (없으면 0 or 무시)
	 * @param OutFailedReasonTags : 실패 이유를 담을 태그 컨테이너 (선택)
	 *
	 * @return true면 장착 가능, false면 장착 불가
	 *
	 * - 실제로는 RSEquipmentManagerComponent가 ASC/AttributeSet에서
	 *   OwnerTags / CharacterLevel / ItemTags를 수집한 뒤,
	 *   이 함수를 호출해서 결과를 판단하는 식으로 사용한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|EquipRequirement")
	bool CheckRequirements(
		const FGameplayTagContainer& OwnerTags,
		const FGameplayTagContainer& ItemTags,
		int32 CharacterLevel,
		FGameplayTagContainer& OutFailedReasonTags
	) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
