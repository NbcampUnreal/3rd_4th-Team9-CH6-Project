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




#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipManagerComponent)

URSEquipManagerComponent::URSEquipManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	ARSCharacter* Char = Cast<ARSCharacter>(GetOwner());
	if (!Char) return;

	CachedASC = Char->GetAbilitySystemComponent(); // 필수

	URSHeroComponent* HeroComp = Char->FindComponentByClass<URSHeroComponent>();
	if (!HeroComp) return;

	HeroComp->OnInputReady.AddLambda([this]()
		{
			HandleInputReady();
		});

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	EquipmentManager = Owner->FindComponentByClass<URSEquipmentManagerComponent>();
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

void URSEquipManagerComponent::OnMainWeaponChanged(URSItemInstance* OldWeapon, URSItemInstance* NewWeapon)
{
	// 1. 무기 패시브 AbilitySet (아이템 자체 AbilitySet) 정리/적용
	ClearItemPassiveAbilitySets();
	ApplyItemPassiveAbilitySets(NewWeapon);

	// 2. CombatStyle (InputOverlay + StyleAbilitySets + AnimLayer 등) 결정/적용
	const URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(NewWeapon);
	ApplyCombatStyle(NewStyle);
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

void URSEquipManagerComponent::HandleEquipmentChanged(
	const FGameplayTag& SlotTag,
	URSItemInstance* OldItem,
	URSItemInstance* NewItem
)
{
	if (!IsWeaponSlot(SlotTag))
	{
		return;
	}

	// 1. SSOT 갱신
	if (NewItem)
	{
		EquippedWeapons.Add(SlotTag, NewItem);
	}
	else
	{
		EquippedWeapons.Remove(SlotTag);
	}

	// 2. "메인 무기 슬롯"만 스타일에 영향
	if (!IsMainWeaponSlot(SlotTag))
	{
		return;
	}

	ClearItemPassiveAbilitySets();
	ApplyItemPassiveAbilitySets(NewItem);

	// 3. CombatStyle 재결정
	const URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(NewItem);
	ApplyCombatStyle(NewStyle);
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
	const FGameplayTag SlotTag = ResolveWeaponSlotFromInputTag(InputTag);
	if (!SlotTag.IsValid())
	{
		return;
	}

	URSItemInstance* Weapon = GetEquippedWeaponBySlot(SlotTag);
	const URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(Weapon);
	ApplyCombatStyle(NewStyle);
}

URSItemInstance* URSEquipManagerComponent::GetEquippedWeaponBySlot(FGameplayTag SlotTag) const
{
	if (const TObjectPtr<URSItemInstance>* Found = EquippedWeapons.Find(SlotTag))
	{
		return Found->Get();
	}
	return nullptr;
}

void URSEquipManagerComponent::SetEquippedWeapon(
	FGameplayTag SlotTag,
	URSItemInstance* NewItem)
{
	URSItemInstance* OldItem = nullptr;

	if (TObjectPtr<URSItemInstance>* Found = EquippedWeapons.Find(SlotTag))
	{
		OldItem = Found->Get();
	}

	if (NewItem)
	{
		EquippedWeapons.Add(SlotTag, NewItem);
	}
	else
	{
		EquippedWeapons.Remove(SlotTag);
	}

	HandleEquipmentChanged(SlotTag, OldItem, NewItem);
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

	// v1: 무기 슬롯 정의를 "명시 2개"로 고정 (Main/Sub)
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