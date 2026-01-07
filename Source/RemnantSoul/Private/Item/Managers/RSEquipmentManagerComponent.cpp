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
	// EquipmentManager는 상태만 관리. 적용(코스메틱 포함)은 EquipManager가 한다.
	// 필요하면 디버그 로그만 남긴다.
	UE_LOG(LogTemp, Verbose, TEXT("[Eq] MainWeaponChanged Old=%s New=%s"),
		*GetNameSafe(OldWeapon), *GetNameSafe(NewWeapon));
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

	// Cosmetic/Apply 호출 금지. EquipManager가 받게 이벤트만 뿌린다.
	URSItemInstance* PayloadItem = nullptr;

	switch (Action)
	{
	case ERSAnimEquipAction::AttachWeapon:
		PayloadItem = PendingNewItem;
		break;
	case ERSAnimEquipAction::DetachWeapon:
		PayloadItem = nullptr;
		break;
	case ERSAnimEquipAction::ApplyStyle:
		// 스타일 적용 트리거는 기존 정책대로 ActiveChanged를 1회 발생시키는 방식으로 처리하거나,
		// 아래처럼 EquipAnimAction 이벤트로 넘겨서 EquipManager가 Decide하도록 한다.
		PayloadItem = PendingNewItem;
		break;
	case ERSAnimEquipAction::ClearStyle:
		PayloadItem = nullptr;
		break;
	default:
		break;
	}

	OnEquipAnimAction.Broadcast(Action, PayloadItem);
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

//void URSEquipmentManagerComponent::SetActiveWeaponSlot(const FGameplayTag& NewActiveSlotTag)
//{
//	// 정책: Active는 항상 Main이므로, 활성 요청은 통일해서 처리
//	RequestActivateWeaponSlot(NewActiveSlotTag);
//}

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
			RequestActivateWeaponSlot(Main);
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

	UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivateWeaponSlot Requested=%s"), *RequestedSlot.ToString());

	if (!RequestedSlot.IsValid() || !IsWeaponSlot(RequestedSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivate INVALID slot"));
		return;
	}

	// 요청 슬롯이 비어있으면 무시
	if (!GetItemInSlot(RequestedSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Eq] RequestActivate slot empty -> ignore"));
		return;
	}

	// --- Old 상태 캡처(브로드캐스트용) ---
	const FGameplayTag OldActiveSlot = ActiveWeaponSlotTag.IsValid() ? ActiveWeaponSlotTag : Main;
	URSItemInstance* OldActiveItem = GetItemInSlot(OldActiveSlot);

	// --- 정책: 손에 드는 무기는 항상 Main ---
	if (RequestedSlot == Sub)
	{
		SwapWeaponSlots(Main, Sub); // SSOT만 교체 + EquipmentChanged 쏠지 여부는 내부 정책(네 코드 유지)
	}

	// Active는 항상 Main으로 고정
	ActiveWeaponSlotTag = Main;

	// --- New 상태 캡처 ---
	URSItemInstance* NewActiveItem = GetItemInSlot(ActiveWeaponSlotTag);

	UE_LOG(LogTemp, Warning, TEXT("[Eq] After RequestActivate Active=%s Main=%s Sub=%s"),
		*ActiveWeaponSlotTag.ToString(),
		*GetNameSafe(GetItemInSlot(Main)),
		*GetNameSafe(GetItemInSlot(Sub)));

	// --- 단 1회 브로드캐스트 ---
	BroadcastActiveWeaponChanged(OldActiveSlot, ActiveWeaponSlotTag, OldActiveItem, NewActiveItem);
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

bool URSEquipmentManagerComponent::IsWeaponSlotFilled(const FGameplayTag& SlotTag) const
{
	return IsWeaponSlot(SlotTag) && (GetItemInSlot(SlotTag) != nullptr);
}