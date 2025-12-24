//#pragma once
//
//#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
//#include "Engine/DataAsset.h"
//#include "GameplayTagContainer.h"
//#include "RSItemTemplate.generated.h"
//
//class ARSWeaponActor;
//class UAnimMontage;
//class USkeletalMesh;
//
//UENUM(BlueprintType)
//enum class ERSWeaponHandType : uint8
//{
//	Unarmed,
//	OneHand,
//	TwoHand,
//	GreatSword,
//	MagicStaff,
//	Bow
//};
//
//UCLASS(BlueprintType)
//class REMNANTSOUL_API URSItemTemplate : public UPrimaryDataAsset
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//	FName ItemId;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//	FText DisplayName;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//	TObjectPtr<UTexture2D> Icon;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//	FGameplayTagContainer ItemTags;
//};
//
//UCLASS(BlueprintType)
//class REMNANTSOUL_API URSItemTemplate : public UObject
//{
//	GENERATED_BODY()
//	
//public:
//	/** 무기 액터 BP (예: B_Weapon_Sword, B_Weapon_Staff) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Cosmetic")
//	TSubclassOf<ARSWeaponActor> WeaponActorClass;
//
//	/** 무기 액터 기본 메쉬 (필요하면 추가하면 됌.) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Cosmetic")
//	TObjectPtr<USkeletalMesh> WeaponMesh;
//
//	/** 장착 소켓 이름 (우리가 사용하고 있는 뼈 메쉬의 손 소켓: hand_rSocket ) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Cosmetic")
//	FName AttachSocketName = TEXT("hand_rSocket");
//
//	/** 한손(OneHand)/양손(Sword + Shield)/대검(GreatSword)/ 마법봉(MagicStaff)/ 화살(Bow)/  등 무기 손 타입 - EssentialAnimation에 있는 실제 애니메이션 기준으로 만들것임.*/
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Cosmetic")
//	ERSWeaponHandType HandType = ERSWeaponHandType::OneHand;
//
//	/** 장착 모션 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Animation")
//	TObjectPtr<UAnimMontage> EquipMontage;
//
//	/** 해제 모션 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Animation")
//	TObjectPtr<UAnimMontage> UnequipMontage;
//
//	/** 무기 타입 태그 (예: Weapon.Sword, Weapon.Staff) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Weapon|Tags", meta = (Categories = "Weapon"))
//	FGameplayTag WeaponTypeTag;
//};



// ItemDataAsset/RSItemTemplate.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSItemTemplate.generated.h"

class URSItemInstance;
class URSItemFragment;

/** RS 아이템 큰 분류 */
UENUM(BlueprintType)
enum class ERSItemType : uint8
{
	Weapon,
	Armor,
	Consumable,
	Material,
	QuestItem,
};

UCLASS(BlueprintType, Const, Abstract)
class REMNANTSOUL_API URSItemTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()

//public:
//	URSItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//#if WITH_EDITOR
//	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
//#endif
//
//public:
//	/** 고유 아이템 ID (세이브/로드, AssetManager 검색 키) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	FName ItemId;
//
//	/** 표시 이름 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	FText DisplayName;
//
//	/** 설명(툴팁) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	FText Description;
//
//	/** 인벤토리/툴팁 아이콘 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	TObjectPtr<UTexture2D> Icon;
//
//	/** 아이템 타입(무기/방어구/소모품 등) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	ERSItemType ItemType = ERSItemType::Weapon;
//
//	/** 이 아이템에 공통적으로 붙는 태그들 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	FGameplayTagContainer ItemTags;
//
//	/** 장착 가능한 슬롯 (예: Slot.Weapon.Main, Slot.Armor.Head 등) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item", meta = (Categories = "Slot"))
//	FGameplayTag SlotTag;
//
//	/** 최대 스택 수 (무기/방어구는 보통 1, 소모품은 99 등) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	int32 MaxStackCount = 1;
//
//	/** (선택) 인벤토리 그리드 가로x세로 (나중에 그리드 인벤토리로 확장할 때 사용 가능) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item")
//	FIntPoint SlotSize = FIntPoint(1, 1);
//
//	/** 이 템플릿에 붙는 Fragment들 (EquipStats / AbilitySet / WeaponCosmetic 등) */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "RS|Item")
//	TArray<TObjectPtr<URSItemFragment>> Fragments;
//
//public:
//	/** 이 템플릿을 기반으로 ItemInstance 생성 */
//	UFUNCTION(BlueprintCallable, Category = "RS|Item")
//	URSItemInstance* CreateItemInstance(AActor* Owner, int32 Count = 1) const;
//
//	/** 특정 Fragment 클래스를 검색 (BP/C++ 공통) */
//	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "RS|Item", meta = (DeterminesOutputType = "FragmentClass"))
//	const URSItemFragment* FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const;
//
//	/** 템플릿 버전 (C++) */
//	template <typename TFragment>
//	const TFragment* FindFragment() const
//	{
//		return Cast<TFragment>(FindFragmentByClass(TFragment::StaticClass()));
//	}
//
//	/** AssetManager용 PrimaryAssetId (TypeName은 프로젝트 설정에 맞춰 조정 가능) */
//	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};












