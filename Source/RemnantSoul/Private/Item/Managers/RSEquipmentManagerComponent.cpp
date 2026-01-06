#include "Item/Managers/RSEquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSCosmeticManagerComponent.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_EquipRequirement.h"

#include "RSGameplayTags.h"

#include "Character/RSCharacter.h"
#include "Character/RSHeroData.h"
#include "Item/Fragments/RSItemFragment_CombatStyle.h"
#include "Character/RSCombatStyleData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentManagerComponent)

URSEquipmentManagerComponent::URSEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 기본값: 메인 슬롯을 Active로 시작 (원하면 정책 변경 가능)
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	ActiveWeaponSlotTag = Tags.Slot_Weapon_Main;

	CacheReferences();


	if (URSCosmeticManagerComponent* Cos = CachedCosmeticManager.Get())
	{
		OnActiveWeaponChanged.AddLambda([Cos](FGameplayTag OldSlot, FGameplayTag NewSlot, URSItemInstance* OldItem, URSItemInstance* NewItem)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Eq->Cos] ActiveWeaponChanged NewItem=%s"), *GetNameSafe(NewItem));
				Cos->ApplyWeaponFromItem(NewItem);
			});
	}

	// DefaultSlots 기반으로 초기 슬롯 세팅
	for (const FGameplayTag& SlotTag : DefaultSlots)
	{
		if (SlotTag.IsValid() && !EquippedItems.Contains(SlotTag))
		{
			EquippedItems.Add(SlotTag, nullptr);
		}
	}

	// 2) 안전장치: 무기 슬롯은 항상 존재
	if (!EquippedItems.Contains(Tags.Slot_Weapon_Main))
		EquippedItems.Add(Tags.Slot_Weapon_Main, nullptr);

	if (!EquippedItems.Contains(Tags.Slot_Weapon_Sub))
		EquippedItems.Add(Tags.Slot_Weapon_Sub, nullptr);
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

	// EquipManager 찾기 (GAS/스탯/Ability 담당을 함.)
	if (URSEquipManagerComponent* EquipMgr = Owner->FindComponentByClass<URSEquipManagerComponent>())
	{
		CachedEquipManager = EquipMgr;
	}

	// CosmeticManager 찾기 (외형 담당을 함.)
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

	// DefaultSlots가 비어 있으면 EquippedItems 기반으로만 관리해도 됨.
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

	// 1. Slot 호환성 체크
	if (!CheckSlotCompatibility(SlotTag, Template, OutFailReason))
	{
		return false;
	}

	// 2. 요구 조건 체크 (EquipRequirement Fragment)
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
		// 정책 : SlotTag가 비어 있으면 장비 불가로 본다.
		OutFailReason = NSLOCTEXT("RS", "Equip_TemplateNoSlotTag", "Item has no slot tag.");
		return false;
	}

	// 1. 엄격 일치
	if (Template->SlotTag == SlotTag)
	{
		return true;
	}

	if (SlotTag.MatchesTag(Template->SlotTag))
	{
		return true;
	}

	// 2. 필요 시 계층 비교로 확장 가능
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
		UE_LOG(LogTemp, Warning, TEXT("[EqReq] No EquipRequirement Fragment -> PASS"));
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

	// 레벨은 아직 Attribute가 없으므로 0으로 전달 (나중에 연동할 예정임.)
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
		UE_LOG(LogTemp, Warning, TEXT("[EqReq] Requirements NOT met. FailedTags=%s"),
			*FailedReasonTags.ToString());

		// 상세한 실패 이유 텍스트를 만들려면 FailedReasonTags를 해석하는 테이블을 두면 좋다.
		// 지금은 일단 공통 메시지로 정리.
		OutFailReason = NSLOCTEXT("RS", "Equip_RequirementsNotMet", "Equip requirements not met.");
		return false;
	}

	return true;
}

bool URSEquipmentManagerComponent::IsWeaponSlot(const FGameplayTag& SlotTag) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	return SlotTag == Tags.Slot_Weapon_Main || SlotTag == Tags.Slot_Weapon_Sub;
}


URSCombatStyleData* URSEquipmentManagerComponent::ResolveDefaultUnarmedStyle() const
{
	const AActor* Owner = GetOwner();
	const ARSCharacter* Char = Cast<ARSCharacter>(Owner);
	if (!Char)
	{
		return nullptr;
	}

	const URSHeroData* HD = Char->GetHeroData();
	if (!HD)
	{
		return nullptr;
	}

	// 너가 말한 구조: HeroData가 DefaultUnarmedStyle을 소유
	return HD->DefaultUnarmedStyle; // <- URSHeroData에 이 멤버가 있어야 함 (이름이 다르면 수정)
}

URSCombatStyleData* URSEquipmentManagerComponent::ResolveCombatStyleForWeaponItem(URSItemInstance* WeaponItem) const
{
	// 무기 없으면 DefaultUnarmed
	if (!WeaponItem)
	{
		return ResolveDefaultUnarmedStyle();
	}

	const URSItemTemplate* Template = WeaponItem->GetTemplate();
	if (!Template)
	{
		return ResolveDefaultUnarmedStyle();
	}

	// 1) WeaponItem의 CombatStyle Fragment를 우선
	const URSItemFragment_CombatStyle* StyleFrag = Template->FindFragment<URSItemFragment_CombatStyle>();
	if (StyleFrag)
	{
		if (URSCombatStyleData* Resolved = StyleFrag->ResolveCombatStyle())
		{
			return Resolved;
		}
	}

	// 2) 못 찾으면 DefaultUnarmed
	return ResolveDefaultUnarmedStyle();
}

void URSEquipmentManagerComponent::HandleMainWeaponChanged(URSItemInstance* OldWeapon, URSItemInstance* NewWeapon)
{
	// GAS/전투 규칙 적용은 금지(ActiveWeaponChanged에서만 적용)

	// Active 슬롯이 메인일 때만 코스메틱 적용
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	if (ActiveWeaponSlotTag != Tags.Slot_Weapon_Main)
	{
		return;
	}

	if (URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get())
	{
		CosMgr->ApplyWeaponFromItem(NewWeapon);
	}
}

void URSEquipmentManagerComponent::ClearEquipTransaction()
{
	bEquipTransactionActive = false;
	PendingSlotTag = FGameplayTag();
	PendingOldItem = nullptr;
	PendingNewItem = nullptr;
}

void URSEquipmentManagerComponent::HandleEquipAnimAction(ERSAnimEquipAction Action)
{
	if (!bEquipTransactionActive)
	{
		return;
	}

	URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get();

	switch (Action)
	{
	case ERSAnimEquipAction::AttachWeapon:
		if (CosMgr)
		{
			CosMgr->ApplyWeaponFromItem(PendingNewItem);
		}
		break;

	case ERSAnimEquipAction::DetachWeapon:
		if (CosMgr)
		{
			CosMgr->ApplyWeaponFromItem(nullptr);
		}
		break;

	case ERSAnimEquipAction::ApplyStyle:
		// EquipManager 직접 호출 금지: ActiveWeaponChanged만 트리거
		if (PendingSlotTag == ActiveWeaponSlotTag)
		{
			OnActiveWeaponChanged.Broadcast(
				ActiveWeaponSlotTag, ActiveWeaponSlotTag,
				PendingOldItem, PendingNewItem
			);
		}
		break;

	case ERSAnimEquipAction::ClearStyle:
		if (PendingSlotTag == ActiveWeaponSlotTag)
		{
			OnActiveWeaponChanged.Broadcast(
				ActiveWeaponSlotTag, ActiveWeaponSlotTag,
				PendingOldItem, nullptr
			);
		}
		break;

	default:
		break;
	}
}



//void URSEquipmentManagerComponent::InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem)
//{
//	URSItemInstance* OldItem = GetItemInSlot(SlotTag);
//
//	EquippedItems.FindOrAdd(SlotTag) = NewItem;
//
//	// 무기 슬롯이면: "즉시 적용" 금지 → 트랜잭션 시작
//	if (IsWeaponSlot(SlotTag))
//	{
//		bEquipTransactionActive = true;
//		PendingSlotTag = SlotTag;
//		PendingOldItem = OldItem;
//		PendingNewItem = NewItem;
//
//		// 여기서 몽타주 재생은 네 정책에 따라:
//		// (A) 아이템 템플릿의 EquipMontage를 Character가 재생
//		// (B) GA로 Equip 능력을 만들어서 재생
//		// 지금 메시지에서는 “Notify 라우팅”이 목적이니까,
//		// 몽타주 재생부는 네가 이미 만들어둔 방식에 연결만 하면 됨.
//
//		// 장비 변경 브로드캐스트를 "즉시" 할지 "몽타주 종료 후" 할지는 정책.
//		// 일단 UI가 빨리 반응하게 하려면 지금처럼 즉시 쏴도 된다.
//		OnEquipmentChanged.Broadcast(SlotTag, OldItem, NewItem);
//		return;
//	}
//
//	// 무기 슬롯이 아닌 장비는 기존 즉시 적용 유지
//	if (URSEquipManagerComponent* EquipMgr = CachedEquipManager.Get())
//	{
//		EquipMgr->HandleEquipmentChanged(SlotTag, OldItem, NewItem);
//	}
//
//	OnEquipmentChanged.Broadcast(SlotTag, OldItem, NewItem);
//}
//
//void URSEquipmentManagerComponent::InternalUnequip(const FGameplayTag& SlotTag)
//{
//	URSItemInstance* OldItem = GetItemInSlot(SlotTag);
//	if (!OldItem && !EquippedItems.Contains(SlotTag))
//	{
//		return;
//	}
//
//	EquippedItems.FindOrAdd(SlotTag) = nullptr;
//
//	if (IsWeaponSlot(SlotTag))
//	{
//		bEquipTransactionActive = true;
//		PendingSlotTag = SlotTag;
//		PendingOldItem = OldItem;
//
//		OnEquipmentChanged.Broadcast(SlotTag, OldItem, nullptr);
//		return;
//	}
//
//	if (URSEquipManagerComponent* EquipMgr = CachedEquipManager.Get())
//	{
//		EquipMgr->HandleEquipmentChanged(SlotTag, OldItem, nullptr);
//	}
//
//	OnEquipmentChanged.Broadcast(SlotTag, OldItem, nullptr);
//}

void URSEquipmentManagerComponent::InternalEquip(const FGameplayTag& SlotTag, URSItemInstance* NewItem)
{
	URSItemInstance* OldItem = GetItemInSlot(SlotTag);

	// 기존 로직: SSOT(EquippedItems) 갱신
	EquippedItems.FindOrAdd(SlotTag) = NewItem;

	// 기존 브로드캐스트 유지
	OnEquipmentChanged.Broadcast(SlotTag, OldItem, NewItem);

	// 핵심 추가: 지금 Active 슬롯이 갱신되었으면 ActiveChanged도 쏜다
	//if (IsWeaponSlot(SlotTag) && SlotTag == ActiveWeaponSlotTag) // 해당 함수 대신 SetActiveWeaponSlot()함수로 한번에 끝내는게 나을듯.
	//{
	//	OnActiveWeaponChanged.Broadcast(SlotTag, SlotTag, OldItem, NewItem);
	//}
}

void URSEquipmentManagerComponent::InternalUnequip(const FGameplayTag& SlotTag)
{
	URSItemInstance* OldItem = GetItemInSlot(SlotTag);

	EquippedItems.FindOrAdd(SlotTag) = nullptr;
	OnEquipmentChanged.Broadcast(SlotTag, OldItem, nullptr);

	// ActiveChanged는 여기서 절대 쏘지 않는다.
	// Active 슬롯 아이템이 빠졌으면, 상위 정책(예: EquipManager or ItemManager)이
	// SetActiveWeaponSlot(Main) 또는 Unarmed로 전환을 호출하게 만든다.
}

URSItemInstance* URSEquipmentManagerComponent::GetWeaponInSlot(int32 SlotIndex) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	if (SlotIndex == 1) return GetItemInSlot(Tags.Slot_Weapon_Main);
	if (SlotIndex == 2) return GetItemInSlot(Tags.Slot_Weapon_Sub);
	return nullptr;
}

URSItemInstance* URSEquipmentManagerComponent::GetActiveWeaponItem() const
{
	if (!ActiveWeaponSlotTag.IsValid())
	{
		return nullptr;
	}
	return GetItemInSlot(ActiveWeaponSlotTag);
}

void URSEquipmentManagerComponent::SetActiveWeaponSlot(const FGameplayTag& NewSlot)
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	const FGameplayTag Main = Tags.Slot_Weapon_Main;

	if (!NewSlot.IsValid())
	{
		return;
	}

	if (!IsWeaponSlot(NewSlot))
	{
		return;
	}

	// 정책상 Active는 항상 Main으로 강제
	const FGameplayTag DesiredActive = Main;

	if (ActiveWeaponSlotTag == DesiredActive)
	{
		// 이미 Main이면, 그래도 아이템이 바뀐 경우 갱신이 필요할 수 있다면 여기서 Broadcast를 할지 정책 결정.
		// 지금은 “슬롯 변경 이벤트”만 쏘는 구조라면 return 유지 가능.
		return;
	}

	const FGameplayTag OldSlot = ActiveWeaponSlotTag;
	URSItemInstance* OldItem = GetItemInSlot(OldSlot);

	ActiveWeaponSlotTag = DesiredActive;
	URSItemInstance* NewItem = GetItemInSlot(ActiveWeaponSlotTag);

	UE_LOG(LogTemp, Warning, TEXT("[Eq] SetActiveWeaponSlot Old=%s New=%s OldItem=%s NewItem=%s"),
		*OldSlot.ToString(), *ActiveWeaponSlotTag.ToString(),
		*GetNameSafe(OldItem), *GetNameSafe(NewItem));

	BroadcastActiveWeaponChanged(OldSlot, ActiveWeaponSlotTag, OldItem, NewItem);

	UE_LOG(LogTemp, Warning, TEXT("[Eq] ActiveChanged -> should call Cosmetic. ActiveItem=%s"),
		*GetNameSafe(GetItemInSlot(ActiveWeaponSlotTag)));
}

void URSEquipmentManagerComponent::BroadcastActiveWeaponChanged(
	const FGameplayTag& OldSlot,
	const FGameplayTag& NewSlot,
	URSItemInstance* OldItem,
	URSItemInstance* NewItem
)
{
	OnActiveWeaponChanged.Broadcast(OldSlot, NewSlot, OldItem, NewItem);
}

void URSEquipmentManagerComponent::SwapWeaponSlots(const FGameplayTag& SlotA, const FGameplayTag& SlotB)
{
	URSItemInstance* ItemA = GetItemInSlot(SlotA);
	URSItemInstance* ItemB = GetItemInSlot(SlotB);

	// InternalEquip는 브로드캐스트도 하니까, 여기서는 SSOT만 바꾸는 “전용 setter”를 두는 게 최적이지만
	// 지금은 단순화를 위해 InternalEquip를 활용해도 됨.
	// 단, Active 슬롯이면 ActiveChanged가 다시 날아갈 수 있으니 순서를 통제해야 함.

	// 가장 안전한 방식: EquippedItems만 직접 바꾸고, 마지막에 필요한 Broadcast만 수동 호출
	EquippedItems.FindOrAdd(SlotA) = ItemB;
	EquippedItems.FindOrAdd(SlotB) = ItemA;

	// UI 갱신용 EquipmentChanged도 필요하면 쏘자(선택)
	OnEquipmentChanged.Broadcast(SlotA, ItemA, ItemB);
	OnEquipmentChanged.Broadcast(SlotB, ItemB, ItemA);
}

bool URSEquipmentManagerComponent::TryPickupWeaponToSlots(URSItemInstance* NewWeaponItem, bool bAutoEquip)
{
	if (!NewWeaponItem) return false;

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	const FGameplayTag Main = Tags.Slot_Weapon_Main;
	const FGameplayTag Sub = Tags.Slot_Weapon_Sub;

	UE_LOG(LogTemp, Warning, TEXT("[Eq] TryPickupWeaponToSlots ENTER Item=%s AutoEquip=%d"),
		*GetNameSafe(NewWeaponItem), bAutoEquip);

	UE_LOG(LogTemp, Warning, TEXT("[Eq] Before Main=%s Sub=%s"),
		*GetNameSafe(GetItemInSlot(Main)),
		*GetNameSafe(GetItemInSlot(Sub)));

	// Main이 비어있으면 Main
	if (!GetItemInSlot(Main))
	{
		FText Fail;
		if (!EquipItemToSlot(Main, NewWeaponItem, Fail))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Eq] EquipItemToSlot(Main) FAILED: %s"), *Fail.ToString());
			return false;
		}

		if (bAutoEquip)
		{
			SetActiveWeaponSlot(Main);
		}

		// 여기! return true 직전
		UE_LOG(LogTemp, Warning, TEXT("[Eq] After(MainPath) Main=%s Sub=%s Active=%s ActiveItem=%s"),
			*GetNameSafe(GetItemInSlot(Main)),
			*GetNameSafe(GetItemInSlot(Sub)),
			*ActiveWeaponSlotTag.ToString(),
			*GetNameSafe(GetActiveWeaponItem()));

		return true;
	}

	// Sub가 비어있으면 Sub
	if (!GetItemInSlot(Sub))
	{
		FText Fail;
		if (!EquipItemToSlot(Sub, NewWeaponItem, Fail))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Eq] EquipItemToSlot(Sub) FAILED: %s"), *Fail.ToString());
			return false;
		}

		if (bAutoEquip)
		{
			RequestActivateWeaponSlot(Sub);
		}

		// 여기! return true 직전
		UE_LOG(LogTemp, Warning, TEXT("[Eq] After(SubPath) Main=%s Sub=%s Active=%s ActiveItem=%s"),
			*GetNameSafe(GetItemInSlot(Main)),
			*GetNameSafe(GetItemInSlot(Sub)),
			*ActiveWeaponSlotTag.ToString(),
			*GetNameSafe(GetActiveWeaponItem()));

		return true;
	}

	// 둘 다 차있으면 Sub 교체
	{
		URSItemInstance* OldSub = GetItemInSlot(Sub);

		UE_LOG(LogTemp, Warning,
			TEXT("[Eq][Pickup] Both occupied. Policy=ReplaceSub OldSub=%s New=%s"),
			*GetNameSafe(OldSub),
			*GetNameSafe(NewWeaponItem));

		FText Fail;
		if (!EquipItemToSlot(Sub, NewWeaponItem, Fail))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Eq] ReplaceSub EquipItemToSlot(Sub) FAILED: %s"), *Fail.ToString());
			return false;
		}

		if (bAutoEquip)
		{
			RequestActivateWeaponSlot(Sub);
		}

		// 여기! return true 직전
		UE_LOG(LogTemp, Warning, TEXT("[Eq] After(ReplaceSubPath) Main=%s Sub=%s Active=%s ActiveItem=%s"),
			*GetNameSafe(GetItemInSlot(Main)),
			*GetNameSafe(GetItemInSlot(Sub)),
			*ActiveWeaponSlotTag.ToString(),
			*GetNameSafe(GetActiveWeaponItem()));

		return true;
	}
}

void URSEquipmentManagerComponent::RequestActivateWeaponSlot(FGameplayTag RequestedSlot)
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	const FGameplayTag Main = Tags.Slot_Weapon_Main;
	const FGameplayTag Sub = Tags.Slot_Weapon_Sub;

	UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivateWeaponSlot Requested=%s"),
		*RequestedSlot.ToString());

	if (!RequestedSlot.IsValid() || !IsWeaponSlot(RequestedSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivate INVALID slot"));
		return;
	}

	URSItemInstance* RequestedItem = GetItemInSlot(RequestedSlot);
	if (!RequestedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivate slot empty -> ignore"));
		return;
	}

	// 정책: 손에 드는 무기 = 항상 Main
	if (RequestedSlot == Sub)
	{
		SwapWeaponSlots(Main, Sub);
		SetActiveWeaponSlot(Main);
	}
	else
	{
		SetActiveWeaponSlot(Main);
	}

	// 타입 안전화: UObject*로 미리 떨구기
	URSItemInstance* MainItem = GetItemInSlot(Main);
	URSItemInstance* SubItem = GetItemInSlot(Sub);

	UE_LOG(LogTemp, Warning, TEXT("[Eq] After RequestActivate Active=%s Main=%s Sub=%s"),
		*ActiveWeaponSlotTag.ToString(),
		*GetNameSafe(MainItem),
		*GetNameSafe(SubItem));

	if (AActor* OwnerActor = GetOwner())
	{
		if (URSCosmeticManagerComponent* Cos = OwnerActor->FindComponentByClass<URSCosmeticManagerComponent>())
		{
			URSItemInstance* ActiveItem = GetItemInSlot(ActiveWeaponSlotTag);

			UE_LOG(LogTemp, Warning, TEXT("[Eq->Cos] ApplyWeaponFromItem ActiveItem=%s"),
				*GetNameSafe(ActiveItem));

			Cos->ApplyWeaponFromItem(ActiveItem);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Eq->Cos] CosmeticManager NOT FOUND on Owner=%s"),
				*GetNameSafe(OwnerActor));
		}
	}
}

void URSEquipmentManagerComponent::SwapMainAndSubIfNeeded(const FGameplayTag& NewSlot)
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	if (NewSlot != Tags.Slot_Weapon_Sub)
	{
		return;
	}

	URSItemInstance* SubItem = GetItemInSlot(Tags.Slot_Weapon_Sub);
	if (!SubItem)
	{
		return;
	}

	URSItemInstance* MainItem = GetItemInSlot(Tags.Slot_Weapon_Main);

	EquippedItems.FindOrAdd(Tags.Slot_Weapon_Main) = SubItem;
	EquippedItems.FindOrAdd(Tags.Slot_Weapon_Sub) = MainItem;
}

