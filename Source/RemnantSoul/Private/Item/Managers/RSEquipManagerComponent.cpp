#include "Item/Managers/RSEquipManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

#include "Character/RSCharacter.h"
#include "Character/RSHeroComponent.h"
#include "Character/RSHeroData.h"
#include "Character/RSCombatStyleData.h"

#include "Input/RSInputConfig.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_AbilitySet.h"
#include "Item/Fragments/RSItemFragment_CombatStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipManagerComponent)

// YKJ Annotation : v1은 ASC LooseTags 사용. "이 스타일이 넣은 태그"만 정확히 빼기 위해 캐시를 둔다.
UPROPERTY(Transient)
FGameplayTagContainer CachedAppliedAnimTags;

URSEquipManagerComponent::URSEquipManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 필요 시 ASC 캐시
	AActor* Owner = GetOwner();
	if (Owner)
	{
		CachedASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
	}

	// 시작 스타일(무기 없음 : EssentialAnimation-Unarmed(정영기 팀원이 구매한 무기 미착용 애니메이션)) 적용: DefaultUnarmed
	ApplyCombatStyle(GetDefaultUnarmedStyle());
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
		// 보강한 형태에 맞춰서 직접 반환하거나 Resolve 함수를 쓰면 됨.
		if (StyleFrag->CombatStyle)
		{
			return StyleFrag->CombatStyle;
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

void URSEquipManagerComponent::ApplyCombatStyle(URSCombatStyleData* NewStyle)
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

	// YKJ Annotation : Style이 들고 있는 LinkedAnimLayerClass를 링크한다.
	if (Style && Style->LinkedAnimLayerClass)
	{
		AnimInst->LinkAnimClassLayers(Style->LinkedAnimLayerClass);
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

void URSEquipManagerComponent::ApplyAnimStyleTags(URSCombatStyleData* Style)
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