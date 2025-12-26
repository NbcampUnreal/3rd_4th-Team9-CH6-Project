#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * CV 스타일: Native GameplayTag 등록(RemnantSoul.cpp에서 사용할 수 있도록 팀장 정영기가 설정해놓음.) + 전역 단일 인스턴스 접근가능함.(RSGameplayTags가 싱글톤 패턴이라는 뜻임.)
 * - 태그 문자열(예: "State.IsJumping")이 정체성이므로, 문자열만 유지하면 기존 에셋/설정은 그대로 살아있음. 여기서 헷갈리면 안되는게 State_IsJumping는 코드상으로 나타내는 게임플레이 태그고, State.IsJumping으로 언리얼에디터 상에서 설정해줘야한다. "_(언더바)" 헷갈리지말것.
 */
struct REMNANTSOUL_API FRSGameplayTags
{
public:
	// CV 방식: 전역 단일 인스턴스
	static FRSGameplayTags GameplayTags;

	static const FRSGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// Ability
	FGameplayTag Ability_Cosmetic_SteamParticle;

	// State
	FGameplayTag State_Activated_SteamParticle;
	FGameplayTag State_IsJumping;
	FGameplayTag State_IsAttacking;
	FGameplayTag State_IsDead;
	FGameplayTag State_Invincible;
	FGameplayTag State_IsSkilling;
	FGameplayTag State_Skill_Cooldown;

	// Data
	FGameplayTag Data_Damage;

	// Event
	FGameplayTag Event_Equip_Weapon;
	FGameplayTag Event_Unequip_Weapon;
	FGameplayTag Event_Skill_CheckHit;

	// GameplayCue
	FGameplayTag GameplayCue_Attack_Hit;
	FGameplayTag GameplayCue_Box_Open;
	FGameplayTag GameplayCue_Damage;
	
	// InputTag
	FGameplayTag InputTag_Native_Move;
	FGameplayTag InputTag_Native_Look;
	FGameplayTag InputTag_Native_Interaction;
	FGameplayTag InputTag_Native_InventoryToggle;
	
	FGameplayTag InputTag_Ability_Jump;
	FGameplayTag InputTag_Ability_Attack;
	FGameplayTag InputTag_Ability_Skill;

private:
	FRSGameplayTags() = default;

	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	static void AddAllTags(UGameplayTagsManager& Manager);
};


/**
 * 
 * // (권장) 기존 코드와 호환되는 브릿지 매크로
 * 
 * 경호튜터님 GAS 프레임워크에서는 기존에 STATE_ISJUMPING 같은 매크로를 많이 써놨다.
 * 아래처럼 Get() 멤버를 참조하게 하면 "호출부 수정 없이" 새 구조로 이행 가능. - 지금 매크로 GameplayTag바꿀 내용이 너무 많아서 아래 내용을 임시로 둠.
 * 완전히 안정화되면 매크로는 제거하고 FRSGameplayTags::Get().XXX로 직접 쓸것임. 이미 HeroComponent쪽은 팀장 정영기가 바꿔놓음.
 */

 // Ability
#define ABILITY_COSMETIC_STEAMPARTICLE (FRSGameplayTags::Get().Ability_Cosmetic_SteamParticle)

// State
#define STATE_ACTIVATED_STEAMPARTICLE (FRSGameplayTags::Get().State_Activated_SteamParticle)
#define STATE_ISJUMPING               (FRSGameplayTags::Get().State_IsJumping)
#define STATE_ISATTACKING             (FRSGameplayTags::Get().State_IsAttacking)
#define STATE_ISDEAD                  (FRSGameplayTags::Get().State_IsDead)
#define STATE_INVINCIBLE              (FRSGameplayTags::Get().State_Invincible)
#define STATE_ISSKILLING              (FRSGameplayTags::Get().State_IsSkilling)
#define STATE_SKILL_COOLDOWN          (FRSGameplayTags::Get().State_Skill_Cooldown)

// Data
#define DATA_DAMAGE                   (FRSGameplayTags::Get().Data_Damage)

// Event
#define EVENT_EQUIP_WEAPON            (FRSGameplayTags::Get().Event_Equip_Weapon)
#define EVENT_UNEQUIP_WEAPON          (FRSGameplayTags::Get().Event_Unequip_Weapon)
#define EVENT_SKILL_CHECKHIT          (FRSGameplayTags::Get().Event_Skill_CheckHit)

// GameplayCue
#define GAMEPLAYCUE_ATTACK_HIT        (FRSGameplayTags::Get().GameplayCue_Attack_Hit)
#define GAMEPLAYCUE_BOX_OPEN          (FRSGameplayTags::Get().GameplayCue_Box_Open)
#define GAMEPLAYCUE_DAMAGE            (FRSGameplayTags::Get().GameplayCue_Damage)
