#include "Item/Managers/RSEquipManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

#include "Character/RSCharacter.h"
#include "Character/RSHeroComponent.h"
#include "Character/RSHeroData.h"
#include "Character/RSCombatStyleData.h"
//#include "RSPlayerStateStructs.h"
#include "PlayerState/RSPlayerState.h"

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

	// 기존 OnInputReady 바인딩은 유지해도 되지만, 이제 필수는 아님.
	if (URSHeroComponent* HeroComp = CachedHeroComponent.Get())
	{
		HeroComp->OnInputReady.AddUObject(this, &ThisClass::HandleInputReady);
	}

	// HeroData 준비 여부는 폴링로 확인
	bHeroDataReady = (GetHeroData() != nullptr);

	// "InputReady + HeroDataReady" 둘 다 될 때까지 짧게 재시도
	GetWorld()->GetTimerManager().SetTimer(InitialGateTimer, this,
		&ThisClass::TryApplyInitialCombatState_Poll, 0.05f, true);
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
	if (bDelegatesBound) return;

	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq) return;

	Eq->OnActiveWeaponChanged.AddUObject(this, &ThisClass::HandleActiveWeaponChanged);

	// 애님 노티파이 기반 Attach/Detach를 EquipManager에서 코스메틱 처리
	Eq->OnEquipAnimAction.AddLambda([this](ERSAnimEquipAction Action, URSItemInstance* Item)
		{
			URSEquipmentManagerComponent* EqLocal = CachedEquipmentManager.Get();
			URSCosmeticManagerComponent* Cos = CachedCosmeticManager.Get();
			if (!EqLocal || !Cos)
			{
				return;
			}

			const FRSGameplayTags& Tags = FRSGameplayTags::Get();
			URSItemInstance* MainItem = EqLocal->GetItemInSlot(Tags.Slot_Weapon_Main);
			URSItemInstance* SubItem = EqLocal->GetItemInSlot(Tags.Slot_Weapon_Sub);

			// Action에 따라 분기할 필요도 사실 없다. "그 순간 슬롯상태"를 다시 그리면 끝.
			Cos->SyncWeaponCosmetics(MainItem, SubItem);
		});

	bDelegatesBound = true;
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
	if (!HeroComp) return;

	// 여기서 Clear하지 말고, 단순히 "적용할 게 없으면 return"
	if (!Style || !Style->OverlayInputConfig)
	{
		return;
	}

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
	bInputReady = true;
	TryApplyInitialCombatState();

	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] HandleInputReady Owner=%s"), *GetNameSafe(GetOwner()));
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

//void URSEquipManagerComponent::HandleEquipSlotInput(FGameplayTag InputTag)
//{
//	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] HandleEquipSlotInput Tag=%s"), *InputTag.ToString());
//
//	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
//
//	FGameplayTag DesiredSlot;
//	if (InputTag == Tags.InputTag_Native_EquipSlot1)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RequestActivate MAIN"));
//		DesiredSlot = Tags.Slot_Weapon_Main;
//	}
//	else if (InputTag == Tags.InputTag_Native_EquipSlot2)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RequestActivate SUB"));
//		DesiredSlot = Tags.Slot_Weapon_Sub;
//	}
//	else
//	{
//		return;
//	}
//
//	EquipWeaponFromSlotTag(DesiredSlot);
//}

void URSEquipManagerComponent::HandleEquipSlotInput(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] HandleEquipSlotInput Tag=%s"), *InputTag.ToString());

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	int32 DesiredIndex = INDEX_NONE;
	if (InputTag == Tags.InputTag_Native_EquipSlot1)
	{
		DesiredIndex = 0; // N키
	}
	else if (InputTag == Tags.InputTag_Native_EquipSlot2)
	{
		DesiredIndex = 1; // M키
	}
	else
	{
		return;
	}

	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq)
	{
		UE_LOG(LogTemp, Error, TEXT("[EquipMgr] EquipmentManager missing. Owner=%s"), *GetNameSafe(GetOwner()));
		return;
	}

	Eq->RequestEquipWeaponByIndex(DesiredIndex);
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

void URSEquipManagerComponent::HandleActiveWeaponChanged(
	FGameplayTag OldSlot, FGameplayTag NewSlot,
	URSItemInstance* OldItem, URSItemInstance* NewItem)
{
	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] ActiveWeaponChanged %s->%s Old=%s New=%s"),
		*OldSlot.ToString(), *NewSlot.ToString(),
		*GetNameSafe(OldItem), *GetNameSafe(NewItem));

	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	URSCosmeticManagerComponent* CosMgr = CachedCosmeticManager.Get();

	// 0) 코스메틱: SSOT(Main/Sub) 기반으로 항상 동기화
	if (Eq && CosMgr)
	{
		const FRSGameplayTags& Tags = FRSGameplayTags::Get();
		URSItemInstance* MainItem = Eq->GetItemInSlot(Tags.Slot_Weapon_Main);
		URSItemInstance* SubItem = Eq->GetItemInSlot(Tags.Slot_Weapon_Sub);

		CosMgr->SyncWeaponCosmetics(MainItem, SubItem);
	}

	// 1) InputReady 이전 방어 (스타일/오버레이/ASC 태그/AnimLayer/Ability는 금지)
	if (URSHeroComponent* Hero = CachedHeroComponent.Get())
	{
		if (!Hero->IsInputInitialized())
		{
			UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] Input not ready -> skip style/abilities"));
			return;
		}
	}

	// 2) Clear -> Apply 순서 고정
	ClearItemPassiveAbilitySets();

	// 3) 새 스타일 결정/적용 (활성 무기 기준)
	const URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(NewItem);
	ApplyCombatStyle(NewStyle);

	// 4) (정책) 활성 무기만 패시브 적용
	if (bApplyItemAbilitySetsAsPassive)
	{
		ApplyItemPassiveAbilitySets(NewItem);
	}
}

void URSEquipManagerComponent::EquipWeaponFromSlot(ERSWeaponSlot Slot)
{
#if RS_LEGACY_ENUM_SLOT_PIPELINE
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	const FGameplayTag SlotTag =
		(Slot == ERSWeaponSlot::Main) ? Tags.Slot_Weapon_Main :
		(Slot == ERSWeaponSlot::Sub) ? Tags.Slot_Weapon_Sub :
		FGameplayTag();

	UE_LOG(LogTemp, Warning, TEXT("[Legacy] EquipWeaponFromSlot(enum) -> SlotTag=%s"), *SlotTag.ToString());

	// 적용은 금지. Tag 파이프라인으로만 라우팅.
	EquipWeaponFromSlotTag(SlotTag);
#else
	UE_LOG(LogTemp, Warning, TEXT("[Legacy] EquipWeaponFromSlot disabled by RS_LEGACY_ENUM_SLOT_PIPELINE=0"));
#endif
}

void URSEquipManagerComponent::OnWeaponPickedUp(URSItemInstance* WeaponInstance)
{
#if RS_LEGACY_ENUM_SLOT_PIPELINE
	if (!WeaponInstance) return;
	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq) return;
	Eq->TryPickupWeaponToSlots(WeaponInstance, /*bAutoEquip=*/true);
#endif
}


void URSEquipManagerComponent::EquipWeaponFromSlotTag(FGameplayTag SlotTag)
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	if (!SlotTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] EquipWeaponFromSlotTag: invalid SlotTag"));
		return;
	}

	if (!(SlotTag == Tags.Slot_Weapon_Main || SlotTag == Tags.Slot_Weapon_Sub))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] EquipWeaponFromSlotTag: not a weapon slot. Slot=%s"),
			*SlotTag.ToString());
		return;
	}

	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq)
	{
		UE_LOG(LogTemp, Error, TEXT("[EquipMgr] EquipWeaponFromSlotTag: CachedEquipmentManager is NULL. Owner=%s"),
			*GetNameSafe(GetOwner()));
		return;
	}

	Eq->RequestActivateWeaponSlot(SlotTag);
}

void URSEquipManagerComponent::RefreshCombatStateFromEquipment()
{
	URSEquipmentManagerComponent* Eq = CachedEquipmentManager.Get();
	if (!Eq)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RefreshCombatStateFromEquipment: EquipmentManager missing"));
		return;
	}

	// 1) 코스메틱은 SSOT(Main/Sub) 기준으로 언제든 동기화 가능
	if (URSCosmeticManagerComponent* Cos = CachedCosmeticManager.Get())
	{
		const FRSGameplayTags& Tags = FRSGameplayTags::Get();
		Cos->SyncWeaponCosmetics(
			Eq->GetItemInSlot(Tags.Slot_Weapon_Main),
			Eq->GetItemInSlot(Tags.Slot_Weapon_Sub)
		);
	}

	// 2) InputReady 이전이면 스타일/어빌리티/오버레이 금지
	URSHeroComponent* HeroComp = CachedHeroComponent.Get();
	if (!HeroComp || !HeroComp->IsInputInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RefreshCombatStateFromEquipment: Input not ready -> skip style"));
		return;
	}

	// 3) “현재 활성 무기(=Main)” 기준으로 Style 결정 (없으면 DefaultUnarmedStyle)
	URSItemInstance* ActiveWeapon = Eq->GetActiveWeaponItem();
	const URSCombatStyleData* Style = ResolveCombatStyleForWeapon(ActiveWeapon);

	// 4) Clear -> Apply 순서 고정
	ClearItemPassiveAbilitySets();
	ApplyCombatStyle(Style);

	// 5) 정책: 활성 무기만 패시브 AbilitySet
	if (bApplyItemAbilitySetsAsPassive)
	{
		ApplyItemPassiveAbilitySets(ActiveWeapon);
	}

	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] RefreshCombatStateFromEquipment Done. ActiveWeapon=%s Style=%s"),
		*GetNameSafe(ActiveWeapon),
		*GetNameSafe(Style));
}

void URSEquipManagerComponent::TryApplyInitialCombatState()
{
	if (!bInputReady || !bHeroDataReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] TryApplyInitialCombatState gated. InputReady=%d HeroDataReady=%d"),
			bInputReady ? 1 : 0, bHeroDataReady ? 1 : 0);
		return;
	}

	// 한번만 초기 적용
	if (bDefaultStyleApplied)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] Initial gate OPEN -> Refresh once"));
	RefreshCombatStateFromEquipment();

	bDefaultStyleApplied = true;
}


void URSEquipManagerComponent::TryApplyInitialCombatState_Poll()
{
	// 1) HeroData gate
	bHeroDataReady = (GetHeroData() != nullptr);

	// 2) Input gate: HeroComponent의 실제 상태로 판정
	URSHeroComponent* Hero = CachedHeroComponent.Get();
	bInputReady = (Hero && Hero->IsInputInitialized());

	// 3) 둘 다 된다면 1회 적용 후 타이머 종료
	if (bInputReady && bHeroDataReady)
	{
		GetWorld()->GetTimerManager().ClearTimer(InitialGateTimer);

		UE_LOG(LogTemp, Warning, TEXT("[EquipMgr] Initial gate OPEN -> apply default style"));
		RefreshCombatStateFromEquipment();
	}
}

void URSEquipManagerComponent::HandleHeroDataReady(const URSHeroData* InHeroData)
{
	bHeroDataReady = (InHeroData != nullptr);
	TryApplyInitialCombatState();
}