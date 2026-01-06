#include "Item/Managers/RSEquipManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

#include "Character/RSCharacter.h"
#include "Character/RSHeroComponent.h"
#include "Character/RSHeroData.h"
#include "Character/RSCombatStyleData.h"
//#include "RSPlayerStateStructs.h"

#include "Input/RSInputConfig.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_AbilitySet.h"
#include "Item/Fragments/RSItemFragment_CombatStyle.h"

#include "RSGameplayTags.h"

#include "Item/Managers/RSEquipmentManagerComponent.h"

#include "Item/Managers/RSCosmeticManagerComponent.h"




#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipManagerComponent)

URSEquipManagerComponent::URSEquipManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheRefs();
	BindDelegates();

	ARSCharacter* Char = Cast<ARSCharacter>(GetOwner());
	if (!Char) return;

	CachedASC = Char->GetAbilitySystemComponent();

	CachedCosmeticManager = Char->FindComponentByClass<URSCosmeticManagerComponent>();

	EquipmentManager = Char->FindComponentByClass<URSEquipmentManagerComponent>();
	if (!EquipmentManager) return;

	// ActiveWeaponChanged 구독 (Native delegate 기준)
	EquipmentManager->OnActiveWeaponChanged.AddUObject(
		this, &ThisClass::HandleActiveWeaponChanged
	);

	// InputReady 이후 DefaultStyle 적용은 유지해도 됨(정책)
	if (URSHeroComponent* HeroComp = Char->FindComponentByClass<URSHeroComponent>())
	{
		HeroComp->OnInputReady.AddLambda([this]() { ApplyDefaultStyleIfNeeded(); });
	}
}

UAbilitySystemComponent* URSEquipManagerComponent::GetASC() const
{
	return CachedASC.Get();
}

const URSCombatStyleData* URSEquipManagerComponent::GetDefaultUnarmedStyle() const
{
	if (const URSHeroData* HeroData = GetHeroData())
	{
		return HeroData->DefaultUnarmedStyle;
	}
	return nullptr;
}

void URSEquipManagerComponent::CacheRefs()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	CachedEquipmentManager = Owner->FindComponentByClass<URSEquipmentManagerComponent>();
	CachedCosmeticManager = Owner->FindComponentByClass<URSCosmeticManagerComponent>();
	CachedHeroComponent = Owner->FindComponentByClass<URSHeroComponent>();
	CachedASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
}

void URSEquipManagerComponent::BindDelegates()
{
	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq) return;

	// 멀티캐스트(네 코드에서 Multicast Delegate) 연결
	Eq->OnActiveWeaponChanged.AddUObject(this, &ThisClass::HandleActiveWeaponChanged);
}

void URSEquipManagerComponent::ApplyItemPassiveAbilitySets(URSItemInstance* Item)
{
	if (!Item) return;

	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	const URSItemTemplate* Template = Item->GetTemplate();
	if (!Template) return;

	const URSItemFragment_AbilitySet* AbilityFrag = Template->FindFragment<URSItemFragment_AbilitySet>();
	if (!AbilityFrag) return;

	// 여러 AbilitySet 부여 (핸들 저장해서 나중에 Clear)
	ItemPassiveHandles.Reset();
	for (const FRSItemAbilitySetEntry& Entry : AbilityFrag->AbilitySets)
	{
		if (!Entry.AbilitySet) continue;

		FRSAbilitySet_GrantedHandles Handles;
		Entry.AbilitySet->GiveToAbilitySystem(ASC, &Handles, Item);
		ItemPassiveHandles.Add(Handles);
	}
}

void URSEquipManagerComponent::ClearItemPassiveAbilitySets()
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) { ItemPassiveHandles.Reset(); return; }

	for (FRSAbilitySet_GrantedHandles& H : ItemPassiveHandles)
	{
		H.TakeFromAbilitySystem(ASC);
	}
	ItemPassiveHandles.Reset();
}

void URSEquipManagerComponent::ApplyStyleInputOverlay(const URSCombatStyleData* Style)
{
	URSHeroComponent* HeroComp = GetHeroComponent();
	if (!HeroComp)
	{
		return;
	}

	if (!Style || !Style->OverlayInputConfig)
	{
		// 스타일이 없거나 오버레이가 없으면 클리어
		HeroComp->ClearOverlayInputConfig();
		return;
	}

	// 중요한점 : 입력 오버레이는 HeroComponent가 실제 적용/바인딩 핸들 관리해줘야함.
	HeroComp->ApplyOverlayInputConfig(Style->OverlayInputConfig);
}

void URSEquipManagerComponent::ClearStyleInputOverlay()
{
	URSHeroComponent* HeroComp = GetHeroComponent();
	if (!HeroComp)
	{
		return;
	}
	HeroComp->ClearOverlayInputConfig();
}



void URSEquipManagerComponent::ApplyAnimStyleLayers(const URSCombatStyleData* Style)
{
	const ARSCharacter* Char = Cast<ARSCharacter>(GetOwner());
	if (!Char) return;

	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return;

	UAnimInstance* AnimInst = Mesh->GetAnimInstance();
	if (!AnimInst) return;

	// YKJ Annotation : 레이어 복귀 정책은 "DefaultUnarmedStyle이 기본 레이어를 가진다"로 고정.
	const URSCombatStyleData* Fallback = GetDefaultUnarmedStyle();
	TSubclassOf<UAnimInstance> LayerClass = nullptr;

	if (Style && Style->LinkedAnimLayerClass)
	{
		LayerClass = Style->LinkedAnimLayerClass;
	}
	else if (Fallback && Fallback->LinkedAnimLayerClass)
	{
		LayerClass = Fallback->LinkedAnimLayerClass;
	}

	if (LayerClass)
	{
		AnimInst->LinkAnimClassLayers(LayerClass);
	}
}

void URSEquipManagerComponent::ClearCurrentCombatStyle()
{
	// YKJ Annotation : Clear는 항상 Apply보다 먼저. (서버/클라 모두 동일한 순서로 유지)
	TakeAbilitySetList(CurrentStyleGrantedHandles);
	ClearStyleInputOverlay();
	ClearAnimStyleTags();

	// AnimLayer 복귀 정책:
	// - 버전 첫번쨰 : "링크 해제" 개념이 애매하니, DefaultUnarmedStyle이 기본 레이어를 갖고 있도록 설계
	ApplyAnimStyleLayers(nullptr);

	CurrentCombatStyle = nullptr;
}

void URSEquipManagerComponent::GiveAbilitySetList(
	const TArray<TObjectPtr<const URSAbilitySet>>& Sets,
	TArray<FRSAbilitySet_GrantedHandles>& OutHandles)
{
	OutHandles.Reset();

	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	for (const URSAbilitySet* Set : Sets)
	{
		if (!Set) continue;

		FRSAbilitySet_GrantedHandles Handles;
		Set->GiveToAbilitySystem(ASC, &Handles, GetOwner());
		OutHandles.Add(Handles);
	}
}

void URSEquipManagerComponent::TakeAbilitySetList(TArray<FRSAbilitySet_GrantedHandles>& InOutHandles)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		InOutHandles.Reset();
		return;
	}

	for (FRSAbilitySet_GrantedHandles& H : InOutHandles)
	{
		H.TakeFromAbilitySystem(ASC);
	}
	InOutHandles.Reset();
}

void URSEquipManagerComponent::ApplyAnimStyleTags(const URSCombatStyleData* Style)
{
	if (!bApplyAnimStyleTagsToASC) return;

	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	CachedAppliedAnimTags.Reset();

	if (!Style) return;

	for (const FGameplayTag& Tag : Style->AnimStyleTags)
	{
		if (!Tag.IsValid()) continue;
		ASC->AddLooseGameplayTag(Tag);
		CachedAppliedAnimTags.AddTag(Tag);
	}
}

void URSEquipManagerComponent::ClearAnimStyleTags()
{
	if (!bApplyAnimStyleTagsToASC) return;

	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) { CachedAppliedAnimTags.Reset(); return; }

	for (const FGameplayTag& Tag : CachedAppliedAnimTags)
	{
		if (!Tag.IsValid()) continue;
		ASC->RemoveLooseGameplayTag(Tag);
	}
	CachedAppliedAnimTags.Reset();
}

void URSEquipManagerComponent::HandleInputReady()
{
	ApplyDefaultStyleIfNeeded();
}

void URSEquipManagerComponent::ApplyDefaultStyleIfNeeded()
{
	if (bDefaultStyleApplied)
	{
		return;
	}

	ARSCharacter* Char = Cast<ARSCharacter>(GetOwner());
	if (!Char) return;

	const URSHeroData* HD = Char->GetHeroData();
	if (!HD || !HD->DefaultUnarmedStyle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Equip] Missing HeroData/DefaultUnarmedStyle. Pawn=%s"), *GetNameSafe(Char));
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[Equip] Apply DefaultUnarmedStyle: %s"),
		*GetNameSafe(HD->DefaultUnarmedStyle));

	// 여기서 네 EquipManager의 기존 ApplyCombatStyle을 호출해야 함
	ApplyCombatStyle(HD->DefaultUnarmedStyle);

	bDefaultStyleApplied = true;
}

const URSCombatStyleData* URSEquipManagerComponent::ResolveCombatStyleForWeapon(const URSItemInstance* WeaponItem) const
{
	// 1️. 무기 없음 → 기본 Unarmed
	if (!WeaponItem)
	{
		return GetDefaultUnarmedStyle();
	}

	// 2️. Template 기반 Fragment 우선
	const URSItemTemplate* Template = WeaponItem->GetTemplate();
	if (Template)
	{
		if (const URSItemFragment_CombatStyle* StyleFrag =
			Template->FindFragment<URSItemFragment_CombatStyle>())
		{
			if (const URSCombatStyleData* Resolved =
				StyleFrag->ResolveCombatStyle())
			{
				return Resolved;
			}
		}
	}

	// 3️. fallback
	return GetDefaultUnarmedStyle();
}

void URSEquipManagerComponent::ApplyCombatStyle(const URSCombatStyleData* NewStyle)
{
	if (CurrentCombatStyle == NewStyle)
	{
		return;
	}

	// 0. 이전 스타일 완전 정리
	ClearCurrentCombatStyle();

	CurrentCombatStyle = NewStyle;

	if (!NewStyle)
	{
		return;
	}

	// 1. AbilitySets 적용 (CombatStyle 고유)
	GiveAbilitySetList(NewStyle->AbilitySets, CurrentStyleGrantedHandles);

	// 2. Input Overlay
	ApplyStyleInputOverlay(NewStyle);

	// 3. Anim Style Tags (ASC)
	ApplyAnimStyleTags(NewStyle);

	// 4. Anim Layer 교체
	ApplyAnimStyleLayers(NewStyle);

	// 5. 디버그/노티파이 (선택)
	OnCombatStyleResolved.Broadcast(NewStyle);
}




ARSCharacter* URSEquipManagerComponent::GetOwnerCharacter() const
{
	return Cast<ARSCharacter>(GetOwner());
}

URSHeroComponent* URSEquipManagerComponent::GetHeroComponent() const
{
	if (ARSCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		return OwnerCharacter->FindComponentByClass<URSHeroComponent>();
	}
	return nullptr;
}

const URSHeroData* URSEquipManagerComponent::GetHeroData() const
{
	if (ARSCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		return OwnerCharacter->GetHeroData();
	}
	return nullptr;
}

void URSEquipManagerComponent::HandleEquipSlotInput(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] HandleEquipSlotInput Tag=%s"), *InputTag.ToString());

	if (!EquipmentManager)
	{
		return;
	}

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	FGameplayTag DesiredSlot;
	if (InputTag == Tags.InputTag_Native_EquipSlot1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RequestActivate MAIN"));
		DesiredSlot = Tags.Slot_Weapon_Main;
	}
	else if (InputTag == Tags.InputTag_Native_EquipSlot2)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RequestActivate SUB"));
		DesiredSlot = Tags.Slot_Weapon_Sub;
	}
	else
	{
		return;
	}

	EquipmentManager->RequestActivateWeaponSlot(DesiredSlot);
}


FGameplayTag URSEquipManagerComponent::ResolveWeaponSlotFromInputTag(FGameplayTag InputTag) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	if (InputTag == Tags.InputTag_Native_EquipSlot1)
	{
		return Tags.Slot_Weapon_Main;
	}
	if (InputTag == Tags.InputTag_Native_EquipSlot2)
	{
		return Tags.Slot_Weapon_Sub;
	}

	return FGameplayTag();
}

bool URSEquipManagerComponent::IsWeaponSlot(const FGameplayTag& SlotTag) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// 무기 슬롯 정의를 "명시 2개"로 고정 (Main/Sub) - Sword랑 Staff 무기 넣어서 사용할 수 있게 만들것임.
	return SlotTag == Tags.Slot_Weapon_Main
		|| SlotTag == Tags.Slot_Weapon_Sub;

	// (확장안) 계층 태그를 쓸 거면 아래처럼 바꿀 수 있음:
	// return SlotTag.MatchesTag(Tags.Slot_Weapon);
}

bool URSEquipManagerComponent::IsMainWeaponSlot(const FGameplayTag& SlotTag) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	return SlotTag == Tags.Slot_Weapon_Main;
}

static URSCombatStyleData* ResolveStyleFromItemOrDefault(AActor* Owner, URSItemInstance* Item)
{
	ARSCharacter* Char = Cast<ARSCharacter>(Owner);
	if (!Char) return nullptr;

	const URSHeroData* HD = Char->GetHeroData();
	URSCombatStyleData* DefaultUnarmed = HD ? HD->DefaultUnarmedStyle : nullptr;

	if (!Item) return DefaultUnarmed;

	const URSItemTemplate* T = Item->GetTemplate();
	if (!T) return DefaultUnarmed;

	const URSItemFragment_CombatStyle* StyleFrag = T->FindFragment<URSItemFragment_CombatStyle>();
	if (!StyleFrag) return DefaultUnarmed;

	URSCombatStyleData* Resolved = StyleFrag->ResolveCombatStyle();
	return Resolved ? Resolved : DefaultUnarmed;
}

//void URSEquipManagerComponent::HandleActiveWeaponChanged(
//	FGameplayTag OldSlot,
//	FGameplayTag NewSlot,
//	URSItemInstance* OldItem,
//	URSItemInstance* NewItem)
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] HandleActiveWeaponChanged %s->%s Old=%s New=%s"),
//		*OldSlot.ToString(), *NewSlot.ToString(),
//		*GetNameSafe(OldItem), *GetNameSafe(NewItem));
//
//	// Cosmetic (활성 무기만)
//	if (URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get())
//	{
//		CosMgr->ApplyWeaponFromItem(NewItem);
//	}
//
//	// Passive (활성 무기만)
//	ClearItemPassiveAbilitySets();
//	ApplyItemPassiveAbilitySets(NewItem);
//
//	// Style (활성 무기만)
//	const URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(NewItem);
//	ApplyCombatStyle(NewStyle);
//}

void URSEquipManagerComponent::HandleActiveWeaponChanged(
	FGameplayTag OldSlot, FGameplayTag NewSlot,
	URSItemInstance* OldItem, URSItemInstance* NewItem)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] ActiveWeaponChanged %s->%s Old=%s New=%s"),
		*OldSlot.ToString(), *NewSlot.ToString(),
		*GetNameSafe(OldItem), *GetNameSafe(NewItem));

	AActor* Owner = GetOwner();

	// 1) 코스메틱 우선 적용 (뷰포트 즉시 피드백)
	if (URSCosmeticManagerComponent* Cos = CachedCosmeticManager.Get())
	{
		Cos->ApplyWeaponFromItem(NewItem);

		UE_LOG(LogTemp, Warning,
			TEXT("[EquipMgr] Cosmetic applied from item=%s"),
			*GetNameSafe(NewItem));
	}

	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] Cosmetic ApplyWeaponFromItem(%s)"),
		*GetNameSafe(NewItem));

	// 2) CombatStyle 결정 (없으면 Unarmed)
	URSCombatStyleData* NewStyle = ResolveStyleFromItemOrDefault(Owner, NewItem);

	UE_LOG(LogTemp, Warning,
		TEXT("[EquipMgr] Style resolved = %s"),
		*GetNameSafe(NewStyle));

	// 3) 입력 Overlay(=Ability 바인딩만) 적용
	//    - 너는 “Overlay IMC는 안 쓴다” 정책이므로, HeroComponent에 Ability 바인딩만 덮어쓰기
	if (URSHeroComponent* Hero = CachedHeroComponent.Get())
	{
		// URSCombatStyleData에 OverlayInputConfig 같은 멤버가 있다고 가정
		// 네 프로젝트 실제 멤버명으로 치환해서 쓰면 됨.
		const URSInputConfig* OverlayConfig = nullptr;
		if (NewStyle)
		{
			OverlayConfig = NewStyle->OverlayInputConfig; // <-- 멤버명 다르면 여기만 바꾸기
		}

		Hero->ApplyOverlayInputConfig(OverlayConfig);

		UE_LOG(LogTemp, Warning,
			TEXT("[EquipMgr] Overlay(AbilityOnly) applied = %s"),
			*GetNameSafe(OverlayConfig));
	}

	// 4) AbilitySet/태그/AnimLayer 적용은 다음 단계에서 확장
	//    (지금은 “보인다 + 입력이 살아있다”를 먼저 완성하는 단계)
}

void URSEquipManagerComponent::EquipWeaponFromSlot(ERSWeaponSlot Slot)
{
	// 1. 이전 무기 해제 (Ability/Input/Anim 전부 포함)
	ClearCurrentWeapon();

	// 2. Slot → WeaponInstance
	URSItemInstance_Weapon* NewWeapon = GetWeaponInSlot(Slot);
	if (!NewWeapon)
	{
		ApplyUnarmedState();
		return;
	}

	// 3. MainSlot SSOT 갱신
	CurrentMainWeapon = NewWeapon;

	// 4. 무기 AbilitySet 적용
	ApplyAbilitySet(NewWeapon);

	// 5. 무기 InputConfig Overlay 적용
	ApplyInputConfig(NewWeapon);

	// 6. 상체 AnimStyle 적용 (ASC LooseTag)
	ApplyAnimStyle(NewWeapon);
}

void URSEquipManagerComponent::OnWeaponPickedUp(URSItemInstance_Weapon* Weapon)
{
	if (!MainSlotWeapon)
	{
		MainSlotWeapon = Weapon;
		EquipWeaponFromSlot(ERSWeaponSlot::Main);
	}
	else if (!SubSlotWeapon)
	{
		SubSlotWeapon = Weapon;
	}
}

URSItemInstance_Weapon* URSEquipManagerComponent::GetWeaponInSlot(ERSWeaponSlot Slot) const
{
	switch (Slot)
	{
	case ERSWeaponSlot::Main:
		return MainSlotWeapon;
	case ERSWeaponSlot::Sub:
		return SubSlotWeapon;
	default:
		return nullptr;
	}
}

FGameplayTag URSEquipManagerComponent::GetSlotTag(ERSWeaponSlot Slot) const
{
	const auto& Tags = FRSGameplayTags::Get();

	return (Slot == ERSWeaponSlot::Main)
		? Tags.Slot_Weapon_Main
		: Tags.Slot_Weapon_Sub;
}

void URSEquipManagerComponent::ClearCurrentWeapon()
{
	// 현재 무기 기준으로 적용된 모든 상태 제거
	ClearItemPassiveAbilitySets();
	ClearCurrentCombatStyle();

	CurrentMainWeapon = nullptr;
}

void URSEquipManagerComponent::ApplyUnarmedState()
{
	// 무기 없음 = DefaultUnarmedStyle 적용
	const URSCombatStyleData* DefaultStyle = GetDefaultUnarmedStyle();
	ApplyCombatStyle(DefaultStyle);
}

void URSEquipManagerComponent::ApplyAbilitySet(URSItemInstance_Weapon* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	// 무기 패시브 AbilitySet 처리
	if (bApplyItemAbilitySetsAsPassive)
	{
		ClearItemPassiveAbilitySets();
		/*ApplyItemPassiveAbilitySets(Weapon);*/
	}
}

void URSEquipManagerComponent::ApplyInputConfig(URSItemInstance_Weapon* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	// 입력은 CombatStyle 기준으로 이미 처리됨
	// (무기별 InputConfig를 쓴다면 여기서 확장)
}

void URSEquipManagerComponent::ApplyAnimStyle(URSItemInstance_Weapon* Weapon)
{
	// AnimStyle은 CombatStyle 단계에서 처리
	// (ASC LooseTag + AnimLayer)
}
