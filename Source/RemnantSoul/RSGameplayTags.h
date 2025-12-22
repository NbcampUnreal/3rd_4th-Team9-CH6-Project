//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameplayTagContainer.h"
//
//struct FRSGameplayTags
//{
//public:
//	// CV 방식: 전역 단일 인스턴스
//	static FRSGameplayTags GameplayTags;
//
//	static const FRSGameplayTags& Get() { return GameplayTags; }
//	static void InitializeNativeTags();
//
//	// ---- State / Data / Event / Cue ----
//	FGameplayTag State_IsJumping;
//	FGameplayTag State_IsAttacking;
//	FGameplayTag State_Invincible;
//
//	FGameplayTag Data_Damage;
//
//	FGameplayTag Event_Equip_Weapon;
//	FGameplayTag Event_Unequip_Weapon;
//
//	FGameplayTag GameplayCue_Attack_Hit;
//
//	// ---- Input Tags ----
//	FGameplayTag InputTag_Native_Move;
//	FGameplayTag InputTag_Native_Look;
//
//	FGameplayTag InputTag_Ability_Jump;
//	FGameplayTag InputTag_Ability_Attack;
//	FGameplayTag InputTag_Ability_Skill;
//
//private:
//	FRSGameplayTags() = default;
//
//	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
//	static void AddAllTags(UGameplayTagsManager& Manager);
//};
//
//#define ABILITY_COSMETIC_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("Ability.Cosmetic.SteamParticle"))
//
//#define STATE_ACTIVATED_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("State.Activated.SteamParticle"))
//
//#define STATE_ISJUMPING FGameplayTag::RequestGameplayTag(FName("State.IsJumping"))
//
//#define STATE_ISATTACKING FGameplayTag::RequestGameplayTag(FName("State.IsAttacking"))
//
//#define DATA_DAMAGE FGameplayTag::RequestGameplayTag(FName("Data.Damage"))
//
//#define STATE_ISDEAD FGameplayTag::RequestGameplayTag(FName("State.IsDead"))
//
//#define STATE_INVINCIBLE FGameplayTag::RequestGameplayTag(FName("State.Invincible"))
//
//#define GAMEPLAYCUE_ATTACK_HIT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Attack.Hit"))
//
//#define GAMEPLAYCUE_BOX_OPEN FGameplayTag::RequestGameplayTag(FName("GameplayCue.Box.Open"))
//
//#define GAMEPLAYCUE_DAMAGE FGameplayTag::RequestGameplayTag(FName("GameplayCue.Damage"))
//
//
//#define EVENT_EQUIP_WEAPON FGameplayTag::RequestGameplayTag(FName("Event.Equip.Weapon"))
//#define EVENT_UNEQUIP_WEAPON FGameplayTag::RequestGameplayTag(FName("Event.Unequip.Weapon"))
//
//#define STATE_ISSKILLING FGameplayTag::RequestGameplayTag(FName("State.IsSkilling"))
//
//#define EVENT_SKILL_CHECKHIT FGameplayTag::RequestGameplayTag(FName("Event.Skill.CheckHit"))
//
//#define STATE_SKILL_COOLDOWN FGameplayTag::RequestGameplayTag(FName("State.Skill.Cooldown"))
//
//
///* InputAction Tag들 */
////#define INPUT_ATTACK FGameplayTag::RequestGameplayTag(TEXT("Input.Attack"))
////#define INPUT_SKILL  FGameplayTag::RequestGameplayTag(TEXT("Input.Skill"))
////#define INPUT_JUMP   FGameplayTag::RequestGameplayTag(TEXT("Input.Jump"))
////#define INPUT_SUPERJUMP   FGameplayTag::RequestGameplayTag(TEXT("Input.SuperJump"))
//
//
//
//class REMNANTSOUL_API RSGameplayTags
//{
//public:
//	RSGameplayTags();
//	~RSGameplayTags();
//};
//


// RSGameplayTags.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * CV 스타일: Native GameplayTag 등록 + 전역 단일 인스턴스 접근
 * - 태그 문자열(예: "State.IsJumping")이 정체성이므로, 문자열만 유지하면 기존 에셋/설정은 그대로 살아있음.
 */
struct REMNANTSOUL_API FRSGameplayTags
{
public:
	// CV 방식: 전역 단일 인스턴스
	static FRSGameplayTags GameplayTags;

	static const FRSGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// ---- Ability ----
	FGameplayTag Ability_Cosmetic_SteamParticle;

	// ---- State ----
	FGameplayTag State_Activated_SteamParticle;
	FGameplayTag State_IsJumping;
	FGameplayTag State_IsAttacking;
	FGameplayTag State_IsDead;
	FGameplayTag State_Invincible;
	FGameplayTag State_IsSkilling;
	FGameplayTag State_Skill_Cooldown;

	// ---- Data ----
	FGameplayTag Data_Damage;

	// ---- Event ----
	FGameplayTag Event_Equip_Weapon;
	FGameplayTag Event_Unequip_Weapon;
	FGameplayTag Event_Skill_CheckHit;

	// ---- GameplayCue ----
	FGameplayTag GameplayCue_Attack_Hit;
	FGameplayTag GameplayCue_Box_Open;
	FGameplayTag GameplayCue_Damage;

	// ---- InputTag ----
	FGameplayTag InputTag_Native_Move;
	FGameplayTag InputTag_Native_Look;

	FGameplayTag InputTag_Ability_Jump;
	FGameplayTag InputTag_Ability_Attack;
	FGameplayTag InputTag_Ability_Skill;

private:
	FRSGameplayTags() = default;

	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	static void AddAllTags(UGameplayTagsManager& Manager);
};


/**
 * ---------------------------
 * (권장) 기존 코드 호환용 브릿지 매크로
 * ---------------------------
 * 기존에 STATE_ISJUMPING 같은 매크로를 많이 써놨다면,
 * 아래처럼 Get() 멤버를 참조하게 하면 "호출부 수정 없이" 새 구조로 이행 가능.
 * 완전히 안정화되면 매크로는 제거하고 FRSGameplayTags::Get().XXX로 직접 쓰는 걸 추천.
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
