#include "Item/Managers/RSEquipManagerComponent.h"

#include "Character/RSCharacter.h"
#include "AbilitySystemComponent.h"

#include "GAS/AS/RSAttributeSet_Character.h"

#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/Fragments/RSItemFragment_EquipStats.h"
#include "Item/Fragments/RSItemFragment_AbilitySet.h"
#include "Item/Fragments/RSItemFragment_WeaponCosmetic.h"
#include "GAS/AS/RSAbilitySet.h"   // URSAbilitySet
#include "Abilities/GameplayAbilityTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipManagerComponent)

URSEquipManagerComponent::URSEquipManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSEquipManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerAndASC();
}

void URSEquipManagerComponent::CacheOwnerAndASC()
{
	if (CachedCharacter.IsValid() && CachedASC.IsValid() && CachedCharAttributes.IsValid())
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (ARSCharacter* RSChar = Cast<ARSCharacter>(Owner))
	{
		CachedCharacter = RSChar;

		if (UAbilitySystemComponent* ASC = RSChar->GetAbilitySystemComponent())
		{
			CachedASC = ASC;

			if (const URSAttributeSet_Character* CharAttrConst = ASC->GetSet<URSAttributeSet_Character>())
			{
				CachedCharAttributes = const_cast<URSAttributeSet_Character*>(CharAttrConst);
			}
		}
	}
}

void URSEquipManagerComponent::HandleEquipmentChanged(const FGameplayTag& SlotTag, URSItemInstance* OldItem, URSItemInstance* NewItem)
{
	if (!CachedASC.IsValid())
	{
		CacheOwnerAndASC();
	}

	// 1) 기존 아이템 효과 제거
	if (OldItem)
	{
		ApplyUnequip(OldItem, SlotTag);
	}

	// 2) 새 아이템 효과 적용
	if (NewItem)
	{
		ApplyEquip(NewItem, SlotTag);
	}
}

void URSEquipManagerComponent::ApplyEquip(URSItemInstance* ItemInstance, const FGameplayTag& SlotTag)
{
	if (!ItemInstance || !CachedASC.IsValid())
	{
		return;
	}

	const URSItemTemplate* Template = ItemInstance->GetTemplate();
	if (!Template)
	{
		return;
	}

	// 1) EquipStats Fragment
	if (const URSItemFragment_EquipStats* EquipStatsFrag = Template->FindFragment<URSItemFragment_EquipStats>())
	{
		ApplyEquipStats(EquipStatsFrag, /*bApply=*/true);
	}

	// 2) AbilitySet Fragment
	if (const URSItemFragment_AbilitySet* AbilityFrag = Template->FindFragment<URSItemFragment_AbilitySet>())
	{
		ApplyAbilitySet(ItemInstance, AbilityFrag, /*bApply=*/true);
	}

	// 3) WeaponCosmetic Fragment의 WeaponTypeTag를 ASC에 Loose Tag로 부여
	if (const URSItemFragment_WeaponCosmetic* WeaponCosFrag = Template->FindFragment<URSItemFragment_WeaponCosmetic>())
	{
		ApplyWeaponTypeTag(WeaponCosFrag, /*bApply=*/true);
	}

	// SlotTag는 나중에 "MainHand/OffHand/Head" 등으로 정책 분기할 때 사용 가능
}

void URSEquipManagerComponent::ApplyUnequip(URSItemInstance* ItemInstance, const FGameplayTag& SlotTag)
{
	if (!ItemInstance || !CachedASC.IsValid())
	{
		return;
	}

	const URSItemTemplate* Template = ItemInstance->GetTemplate();
	if (!Template)
	{
		return;
	}

	// 1) EquipStats Fragment 역적용 (보정치 제거)
	if (const URSItemFragment_EquipStats* EquipStatsFrag = Template->FindFragment<URSItemFragment_EquipStats>())
	{
		ApplyEquipStats(EquipStatsFrag, /*bApply=*/false);
	}

	// 2) AbilitySet Fragment 제거
	if (const URSItemFragment_AbilitySet* AbilityFrag = Template->FindFragment<URSItemFragment_AbilitySet>())
	{
		ApplyAbilitySet(ItemInstance, AbilityFrag, /*bApply=*/false);
	}

	// 3) WeaponTypeTag 제거
	if (const URSItemFragment_WeaponCosmetic* WeaponCosFrag = Template->FindFragment<URSItemFragment_WeaponCosmetic>())
	{
		ApplyWeaponTypeTag(WeaponCosFrag, /*bApply=*/false);
	}
}

void URSEquipManagerComponent::ApplyEquipStats(const URSItemFragment_EquipStats* EquipStatsFrag, bool bApply)
{
	if (!EquipStatsFrag || !CachedASC.IsValid() || !CachedCharAttributes.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = CachedASC.Get();

	const float Sign = bApply ? 1.f : -1.f;

	// AttackRange
	{
		const float CurrentRange = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute());
		const float NewRange = CurrentRange + EquipStatsFrag->AttackRangeBonus * Sign;
		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute(), NewRange);
	}

	// AttackDamage
	{
		const float CurrentDamage = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute());
		const float NewDamage = CurrentDamage + EquipStatsFrag->AttackDamageBonus * Sign;
		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute(), NewDamage);
	}

	// 나중에 Defense / Poise / SkillPower 등 추가되면 동일 패턴으로 확장
}

void URSEquipManagerComponent::ApplyAbilitySet(
	URSItemInstance* ItemInstance,
	const URSItemFragment_AbilitySet* AbilityFrag,
	bool bApply)
{
	if (!ItemInstance || !AbilityFrag || !CachedASC.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = CachedASC.Get();

	if (bApply)
	{
		// 아이템별 Handles 구조체 생성/캐시
		FRSAbilitySet_GrantedHandles& GrantedHandles =
			ItemToGrantedAbilityHandles.FindOrAdd(ItemInstance);

		const TArray<FRSItemAbilitySetEntry>& Entries = AbilityFrag->GetAbilitySets();

		for (const FRSItemAbilitySetEntry& Entry : Entries)
		{
			if (!Entry.AbilitySet)
			{
				continue;
			}

			// Lyra 스타일 호출
			Entry.AbilitySet->GiveToAbilitySystem(
				ASC,
				&GrantedHandles,
				ItemInstance    // SourceObject: 보통 무기/아이템 인스턴스 or 캐릭터
			);
		}
	}
	else
	{
		if (FRSAbilitySet_GrantedHandles* FoundHandles =
			ItemToGrantedAbilityHandles.Find(ItemInstance))
		{
			// Lyra 패턴이라면 이런 함수가 있음(이름은 프로젝트에서 확인 필요)
			FoundHandles->TakeFromAbilitySystem(ASC);

			ItemToGrantedAbilityHandles.Remove(ItemInstance);
		}
	}
}

void URSEquipManagerComponent::ApplyWeaponTypeTag(const URSItemFragment_WeaponCosmetic* WeaponCosFrag, bool bApply)
{
	if (!WeaponCosFrag || !CachedASC.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = CachedASC.Get();

	if (!WeaponCosFrag->WeaponTypeTag.IsValid())
	{
		return;
	}

	if (bApply)
	{
		ASC->AddLooseGameplayTag(WeaponCosFrag->WeaponTypeTag);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(WeaponCosFrag->WeaponTypeTag);
	}
}
