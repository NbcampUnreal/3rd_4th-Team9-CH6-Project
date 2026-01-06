#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSItemTemplate.generated.h"

class URSItemInstance;
class URSItemFragment;
class UTexture2D;

/** 아이템의 대분류 (무기/방어구/소모품 등) */
UENUM(BlueprintType)
enum class ERSItemType : uint8
{
	None        UMETA(DisplayName = "None"),
	Weapon      UMETA(DisplayName = "Weapon"),
	Armor       UMETA(DisplayName = "Armor"),
	Consumable  UMETA(DisplayName = "Consumable"),
	Material    UMETA(DisplayName = "Material"),
	KeyItem     UMETA(DisplayName = "KeyItem"),
};

/**
 * RSItemTemplate
 *
 * - "정적 데이터"를 가진 아이템 템플릿 (이름, 설명, 아이콘, 태그, 스택 규칙, Fragment 등)
 * - 인스턴스(URSItemInstance)는 여기에서 CreateItemInstance로 생성
 * - Fragment 시스템을 통해 무기/장비/스탯/AbilitySet/코스메틱 등을 조합
 *
 *  예)
 *   - Sword_01:
 *       ItemType = Weapon
 *       SlotTag  = Slot.Weapon.Main
 *       MaxStackCount = 1
 *       ItemTags = { Item.Weapon.Sword }
 *       Fragments:
 *         - RSItemFragment_EquipStats
 *         - RSItemFragment_AbilitySet
 *         - RSItemFragment_WeaponCosmetic
 *         - RSItemFragment_EquipRequirement
 */
UCLASS(Blueprintable, Abstract)
class REMNANTSOUL_API URSItemTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URSItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

public:
	/** 템플릿 기반으로 새로운 ItemInstance 생성 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item")
	URSItemInstance* CreateItemInstance(AActor* OwningActor, int32 Count) const;

	/** MaxStackCount를 최소 1로 보정해서 반환 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	int32 GetMaxStackCount() const { return FMath::Max(MaxStackCount, 1); }

	/** 특정 Fragment 클래스를 찾아 반환 (없으면 nullptr) */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "RS|Item", meta = (DeterminesOutputType = "FragmentClass"))
	const URSItemFragment* FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const;

	template<typename TFragmentClass>
	const TFragmentClass* FindFragment() const
	{
		return Cast<TFragmentClass>(FindFragmentByClass(TFragmentClass::StaticClass()));
	}

public:
	/** 내부적/데이터적 아이디 (에디터/저장용 키) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
	FName ItemId;

	/** 표시 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
	FText DisplayName;

	/** 설명 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item", meta = (MultiLine = true))
	FText Description;

	/** UI 아이콘 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
	TObjectPtr<UTexture2D> Icon;

	/** 대분류 타입 (Weapon/Armor/Consumable...) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
	ERSItemType ItemType = ERSItemType::None;

	/**
	 * 이 아이템이 기본적으로 장착되는 슬롯
	 * - 예: Slot.Weapon.Main, Slot.Armor.Head 등
	 * - RSEquipmentManagerComponent에서 SlotTag ↔ SlotTag 호환성 체크에 사용
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item", meta = (Categories = "Slot"))
	FGameplayTag SlotTag;

	/** 최대 스택 개수 (무기/장비는 보통 1, 포션/재료는 N) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item", meta = (ClampMin = "1"))
	int32 MaxStackCount = 1;

	/** 아이템 분류 태그 (예: Item.Weapon.Sword, Item.Element.Fire 등) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item", meta = (Categories = "Item"))
	FGameplayTagContainer ItemTags;

	/** 이 템플릿에 붙은 Fragment들 (EquipStats/AbilitySet/WeaponCosmetic/EquipRequirement 등) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "RS|Item", meta = (TitleProperty = "Class"))
	TArray<TObjectPtr<URSItemFragment>> Fragments;
};











