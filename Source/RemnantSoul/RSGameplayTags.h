#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * Native GameplayTag 등록 + 전역 단일 인스턴스 접근
 *
 * - 태그 문자열(예: "State.IsJumping")이 정체성이므로, 문자열만 유지하면 기존 에셋/설정은 그대로 유지된다.
 * - 코드 변수명(State_IsJumping)과 에디터 문자열(State.IsJumping)은 다르다. 언더바/점 표기 혼동 주의.
 */
struct REMNANTSOUL_API FRSGameplayTags
{
public:
	// 전역 단일 인스턴스
	static FRSGameplayTags GameplayTags;

	static const FRSGameplayTags& Get() { return GameplayTags; }

	/** 프로젝트 부팅 시 1회 호출(StartupModule). 중복 호출되어도 안전하게 방어한다. */
	static void InitializeNativeTags();

	// =========================================================
	// Ability (기존)
	// =========================================================
	FGameplayTag Ability_Cosmetic_SteamParticle;
	FGameplayTag Ability_Sprint;
	FGameplayTag Ability_Sprint_Active;
	FGameplayTag Ability_Sprint_Check;
	FGameplayTag Ability_Crouch;
	FGameplayTag Ability_Roll;
	FGameplayTag Ability_Interact;

	// =========================================================
	// State (기존)
	// =========================================================
	FGameplayTag State_Activated_SteamParticle;
	FGameplayTag State_IsJumping;
	FGameplayTag State_IsAttacking;
	FGameplayTag State_IsDead;
	FGameplayTag State_Invincible;
	FGameplayTag State_IsSkilling;
	FGameplayTag State_Skill_Cooldown;

	// =========================================================
	// Data (기존)
	// =========================================================
	FGameplayTag Data_Damage;

	// Damage.* (데미지 의미/종류)
	FGameplayTag Damage_Type_Light;     // "Damage.Type.Light"
	FGameplayTag Damage_Type_Heavy;     // "Damage.Type.Heavy"

	// Damage.Source.* (무기/스킬 등 출처 구분)
	FGameplayTag Damage_Source_Melee;   // "Damage.Source.Melee"
	FGameplayTag Damage_Source_Skill;   // "Damage.Source.Skill"

	// =========================================================
	// Event (기존)
	// =========================================================
	FGameplayTag Event_Equip_Weapon;
	FGameplayTag Event_Unequip_Weapon;
	FGameplayTag Event_Skill_CheckHit;
	FGameplayTag Event_Roll_End;

	// =========================================================
	// GameplayCue (기존)
	// =========================================================
	FGameplayTag GameplayCue_Attack_Hit;
	FGameplayTag GameplayCue_Box_Open;
	FGameplayTag GameplayCue_Damage;

	// =========================================================
	// InputTag (기존: InputTag.* 네임스페이스)
	// =========================================================
	FGameplayTag InputTag_Native_Move;
	FGameplayTag InputTag_Native_Look;
	FGameplayTag InputTag_Native_InventoryToggle;

	FGameplayTag InputTag_Native_EquipSlot1;
	FGameplayTag InputTag_Native_EquipSlot2;

	FGameplayTag InputTag_Native_QuickSlotCycle;
	FGameplayTag InputTag_Native_QuickSlotUse;
	
	FGameplayTag InputTag_Ability_Jump;
	FGameplayTag InputTag_Ability_SuperJump;
	FGameplayTag InputTag_Ability_Attack;
	FGameplayTag InputTag_Ability_Skill;
	FGameplayTag InputTag_Ability_Sprint;
	FGameplayTag InputTag_Ability_Crouch;
	FGameplayTag InputTag_Ability_Roll;
	FGameplayTag InputTag_Ability_Interact;



	// =========================================================
	// Slot.* (장착 슬롯)
	// =========================================================
	FGameplayTag Slot_Weapon_Main;   // "Slot.Weapon.Main"
	FGameplayTag Slot_Weapon_Sub;    // "Slot.Weapon.Sub" (선택)
	FGameplayTag Slot_Armor_Head;    // "Slot.Armor.Head" (선택)
	FGameplayTag Slot_Armor_Chest;   // "Slot.Armor.Chest" (선택)

	// =========================================================
	// Item.* (아이템 분류/인벤 필터/드랍)
	// =========================================================
	FGameplayTag Item_Weapon_Sword;  // "Item.Weapon.Sword"
	FGameplayTag Item_Weapon_Staff;  // "Item.Weapon.Staff"
	FGameplayTag Item_Consumable;    // "Item.Consumable" (선택)

	// =========================================================
	// Weapon.* (무기 타입/애니 분기 기준)
	// =========================================================
	FGameplayTag Weapon_Sword_OneHand; // "Weapon.Sword.OneHand"
	FGameplayTag Weapon_Staff_OneHand; // "Weapon.Staff.OneHand"
	FGameplayTag Weapon_Staff_TwoHand; // "Weapon.Staff.TwoHand"

	// =========================================================
	// State.AnimStyle.* (ASC LooseTags로 ABP가 읽게 할 스타일)
	// =========================================================
	FGameplayTag State_AnimStyle_Unarmed; // "State.AnimStyle.Unarmed"
	FGameplayTag State_AnimStyle_Sword1H; // "State.AnimStyle.Sword1H"
	FGameplayTag State_AnimStyle_Staff2H; // "State.AnimStyle.Staff2H"

	// =========================================================
	// Status.* (ASC : RSGameplay_Crouch, RSGameplay_Sprint_Action, RSGameplay_Sprint_Check)
	// =========================================================
	FGameplayTag Status_Sprint;
	FGameplayTag Status_Crouch;
	FGameplayTag Status_Roll;

	// =========================================================
	// Input.* (EnhancedInput → AbilityTag 매핑 키)
	// =========================================================
	FGameplayTag Input_Attack_Light;     // "Input.Attack.Light"
	FGameplayTag Input_Attack_Heavy;     // "Input.Attack.Heavy"
	FGameplayTag Input_Skill_Primary;    // "Input.Skill.Primary"
	FGameplayTag Input_Skill_Secondary;  // "Input.Skill.Secondary"
	FGameplayTag Input_Dodge;            // "Input.Dodge"
	FGameplayTag Input_Block;            // "Input.Block"

private:
	FRSGameplayTags() = default;

	/** (A) Manager 주입 방식: 외부에서 전달받은 Manager를 사용해 Native Tag를 등록한다. */
	static void AddTag(UGameplayTagsManager& Manager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

	/** 모든 태그 등록 */
	static void AddAllTags(UGameplayTagsManager& Manager);
};


// =========================================================
// (권장) 기존 코드와 호환되는 브릿지 매크로 (임시)
// =========================================================

//// Ability
//#define ABILITY_COSMETIC_STEAMPARTICLE (FRSGameplayTags::Get().Ability_Cosmetic_SteamParticle)
//
//// State
//#define STATE_ACTIVATED_STEAMPARTICLE (FRSGameplayTags::Get().State_Activated_SteamParticle)
//#define STATE_ISJUMPING               (FRSGameplayTags::Get().State_IsJumping)
//#define STATE_ISATTACKING             (FRSGameplayTags::Get().State_IsAttacking)
//#define STATE_ISDEAD                  (FRSGameplayTags::Get().State_IsDead)
//#define STATE_INVINCIBLE              (FRSGameplayTags::Get().State_Invincible)
//#define STATE_ISSKILLING              (FRSGameplayTags::Get().State_IsSkilling)
//#define STATE_SKILL_COOLDOWN          (FRSGameplayTags::Get().State_Skill_Cooldown)
//
//// Data
//#define DATA_DAMAGE                   (FRSGameplayTags::Get().Data_Damage)
//
//// Event
//#define EVENT_EQUIP_WEAPON            (FRSGameplayTags::Get().Event_Equip_Weapon)
//#define EVENT_UNEQUIP_WEAPON          (FRSGameplayTags::Get().Event_Unequip_Weapon)
//#define EVENT_SKILL_CHECKHIT          (FRSGameplayTags::Get().Event_Skill_CheckHit)
//
//// GameplayCue
//#define GAMEPLAYCUE_ATTACK_HIT        (FRSGameplayTags::Get().GameplayCue_Attack_Hit)
//#define GAMEPLAYCUE_BOX_OPEN          (FRSGameplayTags::Get().GameplayCue_Box_Open)
//#define GAMEPLAYCUE_DAMAGE            (FRSGameplayTags::Get().GameplayCue_Damage)
