#pragma once

#include "CoreMinimal.h"
#include "Item/RSItemTemplate.h"
#include "RSItemTemplate_Weapon.generated.h"

class UAnimMontage;
class USkeletalMesh;
class AActor;

/**
 * 무기 손 타입
 * - 한손 / 양손 / 쌍수 등
 * - 전투 애님 레이어, 장비 슬롯 로직에서 참고
 */
UENUM(BlueprintType)
enum class ERSWeaponHandType : uint8
{
	OneHand UMETA(DisplayName = "One Hand"),
	TwoHand UMETA(DisplayName = "Two Hand"),
	Dual    UMETA(DisplayName = "Dual Wield"),
};

/**
 * 무기 전용 템플릿
 *
 * - URSItemTemplate를 상속받아 "무기라는 의미"와
 *   "무기 타입/손 타입" 같은 분류 정보를 추가로 보관
 *
 * - 실제 코스메틱/스폰/소켓/몽타주는
 *   RSItemFragment_WeaponCosmetic에서 관리하는 방향을 추천
 *   (Single Source of Truth 유지)
 */
UCLASS(BlueprintType)
class REMNANTSOUL_API URSItemTemplate_Weapon : public URSItemTemplate
{
	GENERATED_BODY()

public:
	URSItemTemplate_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/** 무기 타입 태그 (예: Weapon.Sword.OneHand, Weapon.Staff.TwoHand) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon", meta = (Categories = "Weapon"))
	FGameplayTag WeaponTypeTag;

	/** 한손/양손/쌍수 등 무기 손 타입 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon")
	ERSWeaponHandType HandType = ERSWeaponHandType::OneHand;

	/**
	 * 이 무기가 기본적으로 사용되는 무기 슬롯 타입
	 * - 예: Slot.Weapon.Main, Slot.Weapon.Sub 등
	 * - 기본 SlotTag(URSItemTemplate::SlotTag)와 중복될 수 있지만,
	 *   논리적으로 "이건 무기 전용 슬롯"이라는 의미를 명확히 하는 용도
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon", meta = (Categories = "Slot"))
	FGameplayTag PreferredWeaponSlotTag;

public:
	/** 편의 함수: 이 템플릿이 Sword인지, Staff인지 등을 빠르게 체크할 때 사용 가능 */
	UFUNCTION(BlueprintPure, Category = "RS|Weapon")
	bool IsWeaponType(FGameplayTag InWeaponTypeTag) const;

	/** 편의 함수: 한손 무기인지 */
	UFUNCTION(BlueprintPure, Category = "RS|Weapon")
	bool IsOneHanded() const { return HandType == ERSWeaponHandType::OneHand; }

	/** 편의 함수: 양손 무기인지 */
	UFUNCTION(BlueprintPure, Category = "RS|Weapon")
	bool IsTwoHanded() const { return HandType == ERSWeaponHandType::TwoHand; }

	/** 편의 함수: 쌍수 무기인지 */
	UFUNCTION(BlueprintPure, Category = "RS|Weapon")
	bool IsDualWield() const { return HandType == ERSWeaponHandType::Dual; }
};
