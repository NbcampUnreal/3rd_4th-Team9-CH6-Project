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

URSCombatStyleData* URSEquipManagerComponent::GetDefaultUnarmedStyle() const
{
	const ARSCharacter* Char = Cast<ARSCharacter>(GetOwner());
	if (!Char) return nullptr;

	const URSHeroData* HD = Char->GetHeroData();
	if (!HD) return nullptr;

	return HD->DefaultUnarmedStyle;
}

URSCombatStyleData* URSEquipManagerComponent::ResolveCombatStyleForWeapon(URSItemInstance* WeaponItem) const
{
	if (!WeaponItem)
	{
		return GetDefaultUnarmedStyle();
	}

	const URSItemTemplate* Template = WeaponItem->GetTemplate();
	if (!Template)
	{
		return GetDefaultUnarmedStyle();
	}

	// Fragment_CombatStyle 우선적그로 처리하자.
	if (const URSItemFragment_CombatStyle* StyleFrag = Template->FindFragment<URSItemFragment_CombatStyle>())
	{
		if (URSCombatStyleData* Resolved = StyleFrag->ResolveCombatStyle())
		{
			return Resolved;
		}
	}


	return GetDefaultUnarmedStyle();
}

void URSEquipManagerComponent::OnMainWeaponChanged(URSItemInstance* OldWeapon, URSItemInstance* NewWeapon)
{
	// 1. 무기 패시브 AbilitySet (아이템 자체 AbilitySet) 정리/적용
	ClearItemPassiveAbilitySets();
	ApplyItemPassiveAbilitySets(NewWeapon);

	// 2. CombatStyle (InputOverlay + StyleAbilitySets + AnimLayer 등) 결정/적용
	URSCombatStyleData* NewStyle = ResolveCombatStyleForWeapon(NewWeapon);
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

void URSEquipManagerComponent::ApplyCombatStyle(const URSCombatStyleData* NewStyle)
{
	if (!NewStyle)
	{
		NewStyle = GetDefaultUnarmedStyle();
	}

	if (CurrentStyle == NewStyle)
	{
		return;
	}

	// 1) 이전 스타일 정리
	ClearCurrentCombatStyle();

	// 2) 교체
	CurrentStyle = NewStyle;

	// 3) 새 스타일 적용
	if (UAbilitySystemComponent* ASC = GetASC())
	{
		if (CurrentStyle)
		{
			GiveAbilitySetList(CurrentStyle->AbilitySets, CurrentStyleGrantedHandles);
		}
	}

	ApplyStyleInputOverlay(CurrentStyle);
	ApplyAnimStyleLayers(CurrentStyle);
	ApplyAnimStyleTags(CurrentStyle);

	// 4) 통지(AnimBP/UI/CameraMode 등에서 구독)
	if (ARSCharacter* Char = Cast<ARSCharacter>(GetOwner()))
	{
		Char->OnCombatStyleChanged(CurrentStyle);
	}
}

URSHeroComponent* URSEquipManagerComponent::GetHeroComponent() const
{
	AActor* Owner = GetOwner();
	ARSCharacter* Char = Cast<ARSCharacter>(Owner);
	if (!Char) return nullptr;

	// HeroComponent는 CreateDefaultSubobject로 붙어있음
	// (접근 함수 없으면 FindComponentByClass로 찾아도 됨)
	return Char->FindComponentByClass<URSHeroComponent>();
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
	// TODO : 장비 변경 처리(아이템 패시브, 스타일 교체 등)
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

	CurrentStyle = nullptr;
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

ERSWeaponSlot URSEquipManagerComponent::ConvertInputTagToSlot(
	const FGameplayTag& InputTag) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	if (InputTag == Tags.InputTag_Native_EquipSlot1)
	{
		return ERSWeaponSlot::Slot1;
	}
	if (InputTag == Tags.InputTag_Native_EquipSlot2)
	{
		return ERSWeaponSlot::Slot2;
	}

	return ERSWeaponSlot::None;
}

void URSEquipManagerComponent::SetCurrentState(
	ERSWeaponSlot NewSlot,
	URSItemInstance* NewWeapon)
{
	CurrentWeaponState.ActiveSlot = NewSlot;
	CurrentWeaponState.ActiveWeapon = NewWeapon;
}

void URSEquipManagerComponent::HandleEquipSlotInput(
	const FGameplayTag& InputTag)
{
	if (!EquipmentManager || !GetDefaultUnarmedStyle())
	{
		return;
	}

	const ERSWeaponSlot PressedSlot = ConvertInputTagToSlot(InputTag);
	if (PressedSlot == ERSWeaponSlot::None)
	{
		return;
	}

	// 같은 슬롯 다시 누름 → Unarmed
	if (PressedSlot == CurrentWeaponState.ActiveSlot)
	{
		if (!EquipmentManager || !GetDefaultUnarmedStyle())
		{
			return;
		}

		ApplyCombatStyle(GetDefaultUnarmedStyle());
		SetCurrentState(ERSWeaponSlot::None, nullptr);
		return;
	}

	URSItemInstance* Weapon =
		EquipmentManager->GetWeaponInSlot(static_cast<int32>(PressedSlot));

	if (!Weapon)
	{
		return;
	}

	const URSCombatStyleData* WeaponStyle =
		ResolveCombatStyleForWeapon(Weapon);

	if (!WeaponStyle)
	{
		return;
	}

	ApplyCombatStyle(GetDefaultUnarmedStyle());
	SetCurrentState(PressedSlot, Weapon);
}