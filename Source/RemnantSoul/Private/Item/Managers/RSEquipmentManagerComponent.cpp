#include "Item/Managers/RSEquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSCosmeticManagerComponent.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_EquipRequirement.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentManagerComponent)

URSEquipmentManagerComponent::URSEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheReferences();

	// DefaultSlots 기반으로 초기 슬롯 세팅
	for (const FGameplayTag& SlotTag : DefaultSlots)
	{
		if (SlotTag.IsValid() && !EquippedItems.Contains(SlotTag))
		{
			EquippedItems.Add(SlotTag, nullptr);
		}
	}
}

void URSEquipmentManagerComponent::CacheReferences()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// ASC 찾기
	if (UAbilitySystemComponent* ASC = Owner->FindComponentByClass<UAbilitySystemComponent>())
	{
		CachedASC = ASC;
	}

	// EquipManager 찾기 (GAS/스탯/Ability 담당)
	if (URSEquipManagerComponent* EquipMgr = Owner->FindComponentByClass<URSEquipManagerComponent>())
	{
		CachedEquipManager = EquipMgr;
	}

	// CosmeticManager 찾기 (외형 담당)
	if (URSCosmeticManagerComponent* CosMgr = Owner->FindComponentByClass<URSCosmeticManagerComponent>())
	{
		CachedCosmeticManager = CosMgr;
	}
}

bool URSEquipmentManagerComponent::HasSlot(const FGameplayTag& SlotTag) const
{
	if (!SlotTag.IsValid())
	{
		return false;
	}

	// DefaultSlots가 비어 있으면 EquippedItems 기반으로만 관리해도 됨
	if (DefaultSlots.Num() == 0)
	{
		return EquippedItems.Contains(SlotTag);
	}

	return DefaultSlots.Contains(SlotTag);
}

URSItemInstance* URSEquipmentManagerComponent::GetItemInSlot(const FGameplayTag& SlotTag) const
{
	if (!SlotTag.IsValid())
	{
		return nullptr;
	}

	if (URSItemInstance* const* Found = EquippedItems.Find(SlotTag))
	{
		return *Found;
	}

	return nullptr;
}

bool URSEquipmentManagerComponent::CanEquipItemToSlot(const FGameplayTag& SlotTag, URSItemInstance* ItemInstance, FText& OutFailReason) const
{
	OutFailReason = FText::GetEmpty();

	if (!SlotTag.IsValid())
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_InvalidSlotTag", "Invalid equipment slot.");
		return false;
	}

	if (!ItemInstance)
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_InvalidItemInstance", "Invalid item instance.");
		return false;
	}

	if (!HasSlot(SlotTag))
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_SlotNotAvailable", "This slot does not exist.");
		return false;
	}

	const URSItemTemplate* Template = ItemInstance->GetTemplate();
	if (!Template)
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_NoTemplate", "Item has no template.");
		return false;
	}

	// 1) Slot 호환성 체크
	if (!CheckSlotCompatibility(SlotTag, Template, OutFailReason))
	{
		return false;
	}

	// 2) 요구 조건 체크 (EquipRequirement Fragment)
	if (!CheckEquipRequirements(Template, OutFailReason))
	{
		return false;
	}

	return true;
}

bool URSEquipmentManagerComponent::EquipItemToSlot(const FGameplayTag& SlotTag, URSItemInstance* ItemInstance, FText& OutFailReason)
{
	if (!CanEquipItemToSlot(SlotTag, ItemInstance, OutFailReason))
	{
		return false;
	}

	InternalEquip(SlotTag, ItemInstance);
	return true;
}

bool URSEquipmentManagerComponent::UnequipItemFromSlot(const FGameplayTag& SlotTag)
{
	if (!SlotTag.IsValid())
	{
		return false;
	}

	if (!HasSlot(SlotTag))
	{
		return false;
	}

	InternalUnequip(SlotTag);
	return true;
}

void URSEquipmentManagerComponent::InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem)
{
	// 기존 아이템 : 정영기 팀원이 만든내용, 기인님이 만든 내용아님. 
	URSItemInstance* OldItem = GetItemInSlot(SlotTag);

	// 맵 갱신
	EquippedItems.FindOrAdd(SlotTag) = NewItem;

	// GAS/스탯/Ability 처리: RSEquipManager에 위임
	if (URSEquipManagerComponent* EquipMgr = CachedEquipManager.Get())
	{
		EquipMgr->HandleEquipmentChanged(SlotTag, OldItem, NewItem);
	}

	// 코스메틱 처리: 무기 슬롯일 때만
	if (IsWeaponSlot(SlotTag))
	{
		if (URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get())
		{
			CosMgr->ApplyWeaponFromItem(NewItem);
		}
	}

	// 델리게이트 브로드캐스트 (UI/로그용)
	OnEquipmentChanged.Broadcast(SlotTag, OldItem, NewItem);
}

void URSEquipmentManagerComponent::InternalUnequip(const FGameplayTag& SlotTag)
{
	URSItemInstance* OldItem = GetItemInSlot(SlotTag);
	if (!OldItem && !EquippedItems.Contains(SlotTag))
	{
		return;
	}

	EquippedItems.FindOrAdd(SlotTag) = nullptr;

	if (URSEquipManagerComponent* EquipMgr = CachedEquipManager.Get())
	{
		EquipMgr->HandleEquipmentChanged(SlotTag, OldItem, nullptr);
	}

	if (IsWeaponSlot(SlotTag))
	{
		if (URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get())
		{
			CosMgr->ApplyWeaponFromItem(nullptr);
		}
	}

	OnEquipmentChanged.Broadcast(SlotTag, OldItem, nullptr);
}

bool URSEquipmentManagerComponent::CheckSlotCompatibility(const FGameplayTag& SlotTag, const URSItemTemplate* Template, FText& OutFailReason) const
{
	OutFailReason = FText::GetEmpty();

	if (!Template)
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_NoTemplate_Internal", "Item has no template.");
		return false;
	}

	// 템플릿이 선언한 SlotTag와 실제 장착 슬롯이 호환되는지 확인
	if (!Template->SlotTag.IsValid())
	{
		// 정책: SlotTag가 비어 있으면 장비 불가로 본다.
		OutFailReason = NSLOCTEXT("RS", "Equip_TemplateNoSlotTag", "Item has no slot tag.");
		return false;
	}

	// 1) 엄격 일치
	if (Template->SlotTag == SlotTag)
	{
		return true;
	}

	// 2) 필요 시 계층 비교로 확장 가능
	// if (Template->SlotTag.MatchesTag(SlotTag) || SlotTag.MatchesTag(Template->SlotTag))
	// {
	//     return true;
	// }

	OutFailReason = NSLOCTEXT("RS", "Equip_IncompatibleSlot", "Item cannot be equipped in this slot.");
	return false;
}

bool URSEquipmentManagerComponent::CheckEquipRequirements(const URSItemTemplate* Template, FText& OutFailReason) const
{
	OutFailReason = FText::GetEmpty();

	if (!Template)
	{
		return false;
	}

	// EquipRequirement Fragment가 없다면 요구조건 없음 → 항상 통과
	const URSItemFragment_EquipRequirement* ReqFrag = Template->FindFragment<URSItemFragment_EquipRequirement>();
	if (!ReqFrag)
	{
		return true;
	}

	if (!CachedASC.IsValid())
	{
		OutFailReason = NSLOCTEXT("RS", "Equip_NoASC", "Cannot verify equip requirements.");
		return false;
	}

	UAbilitySystemComponent* ASC = CachedASC.Get();

	// ASC가 가진 태그
	FGameplayTagContainer OwnerTags;
	ASC->GetOwnedGameplayTags(OwnerTags);

	// 템플릿에 설정된 ItemTags
	const FGameplayTagContainer& ItemTags = Template->ItemTags;

	// 레벨은 아직 Attribute가 없으므로 0으로 전달 (나중에 연동)
	const int32 CharacterLevel = 0;

	FGameplayTagContainer FailedReasonTags;
	const bool bOK = ReqFrag->CheckRequirements(
		OwnerTags,
		ItemTags,
		CharacterLevel,
		FailedReasonTags
	);

	if (!bOK)
	{
		// 상세한 실패 이유 텍스트를 만들려면 FailedReasonTags를 해석하는 테이블을 두면 좋다.
		// 지금은 일단 공통 메시지로 정리.
		OutFailReason = NSLOCTEXT("RS", "Equip_RequirementsNotMet", "Equip requirements not met.");
		return false;
	}

	return true;
}

bool URSEquipmentManagerComponent::IsWeaponSlot(const FGameplayTag& SlotTag) const
{
	if (!SlotTag.IsValid() || !MainWeaponSlotTag.IsValid())
	{
		return false;
	}

	// 단순히 "메인 무기 슬롯"만 무기 슬롯으로 취급
	if (SlotTag == MainWeaponSlotTag)
	{
		return true;
	}

	// 필요하면 Slot.Weapon.* 계층을 쓰면서 확장 가능
	// if (SlotTag.MatchesTag(MainWeaponSlotTag))
	// {
	//     return true;
	// }

	return false;
}
