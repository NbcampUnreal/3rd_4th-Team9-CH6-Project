#include "Item/RSItemTemplate_Weapon.h"

#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemTemplate_Weapon)

URSItemTemplate_Weapon::URSItemTemplate_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 무기 템플릿은 기본적으로 Weapon 타입 + MaxStackCount = 1로 고정하는 쪽을 추천
	ItemType = ERSItemType::Weapon;
	MaxStackCount = 1;

	// 필요하다면 여기서 기본 SlotTag/ItemTags를 세팅해도 된다.
	// 예를 들어 RSGameplayTags에 Slot.Weapon.Main, Item.Weapon.Sword 등을
	// Native로 추가해두고, 생성자에서 기본값을 넣는 식.
	//
	// const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	// PreferredWeaponSlotTag = Tags.Slot_Weapon_Main;  // 가정
	// SlotTag                = PreferredWeaponSlotTag;
}

bool URSItemTemplate_Weapon::IsWeaponType(FGameplayTag InWeaponTypeTag) const
{
	if (!InWeaponTypeTag.IsValid())
	{
		return false;
	}

	// 1) 본인 전용 WeaponTypeTag와 비교
	if (WeaponTypeTag.IsValid() && WeaponTypeTag.MatchesTagExact(InWeaponTypeTag))
	{
		return true;
	}

	// 2) 템플릿 공통 ItemTags에도 무기 타입 태그를 포함시켰다면,
	//    여기에서 함께 체크 가능
	if (ItemTags.HasTag(InWeaponTypeTag))
	{
		return true;
	}

	return false;
}
