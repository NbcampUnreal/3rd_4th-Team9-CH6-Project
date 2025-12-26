#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_WeaponCosmetic.generated.h"

class ARSWeaponActor;
class USkeletalMesh;
class UAnimMontage;

/**
 * 무기 코스메틱/장착 정보를 담는 Fragment
 *
 * - 어떤 WeaponActor BP를 스폰할지
 * - 어떤 SkeletalMesh를 쓸지 (필요 시)
 * - 어떤 소켓에 붙일지
 * - 장착/해제 몽타주는 무엇인지
 * - 무기 타입 태그 (Sword / Staff 등)
 *
 * 실제 스폰/어태치/모션 재생은
 *   - RSCosmeticManagerComponent
 *   - RSEquipManagerComponent
 * 쪽에서 이 Fragment를 읽어서 처리한다.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_WeaponCosmetic : public URSItemFragment
{
	GENERATED_BODY()
	// YKJ Annotation : 
public:
	URSItemFragment_WeaponCosmetic(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/** 무기 액터 BP (예: B_Weapon_Sword, B_Weapon_Staff) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Cosmetic")
	TSubclassOf<ARSWeaponActor> WeaponActorClass;

	/**
	 * 무기 메쉬 (필요할 때만 사용)
	 * - WeaponActorClass가 자체적으로 메쉬를 들고 있다면 생략 가능
	 * - 단순히 캐릭터 Mesh의 소켓에 바로 붙이는 구조라면 이 메쉬를 사용
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Cosmetic")
	TObjectPtr<USkeletalMesh> WeaponMesh;

	/** 장착 시 붙일 소켓명 (예: hand_rSocket, WeaponSocket 등) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Cosmetic")
	FName AttachSocketName = TEXT("hand_rSocket");

	/**
	 * 휴대(홀스터) 상태 소켓명 (선택)
	 * - 예: back_socket, waist_l_socket
	 * - 나중에 "홀스터 상태 → 손으로 들기" 전환 연출을 할 때 사용 가능
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Cosmetic")
	FName HolsterSocketName;

	/**
	 * 장착 모션 (Equip)
	 * - 예: 검 뽑는 모션, 스태프 들어올리는 모션
	 * - 없으면 그냥 소켓에 순간 이동으로 붙여도 됨
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Animation")
	TObjectPtr<UAnimMontage> EquipMontage;

	/**
	 * 해제 모션 (Unequip)
	 * - 예: 검 집어넣기, 스태프 내려놓기 모션
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Animation")
	TObjectPtr<UAnimMontage> UnequipMontage;

	/**
	 * 무기 타입 태그
	 * - 예: Weapon.Sword, Weapon.Staff
	 * - RSGameplayTags 말고, 아이템 분류용 Weapon 카테고리 태그
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Weapon|Tags", meta = (Categories = "Weapon"))
	FGameplayTag WeaponTypeTag;

public:
	/** 이 Fragment가 코스메틱적으로 유효한지 간단 체크 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Weapon|Cosmetic")
	bool IsValidForEquip() const;

	/** 장착/해제 모션이 있는지 확인용 유틸 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Weapon|Cosmetic")
	bool HasEquipMontage() const { return EquipMontage != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "RS|Item|Weapon|Cosmetic")
	bool HasUnequipMontage() const { return UnequipMontage != nullptr; }

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
