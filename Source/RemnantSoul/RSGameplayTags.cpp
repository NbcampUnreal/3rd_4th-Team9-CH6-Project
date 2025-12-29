#include "RSGameplayTags.h"
#include "GameplayTagsManager.h"

FRSGameplayTags FRSGameplayTags::GameplayTags;

void FRSGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	AddAllTags(Manager);
}

void FRSGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName(TagName),
		FString(TEXT("(Native) ")) + FString(TagComment)
	);
}

void FRSGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// =========================================================
	// Ability (기존)
	// =========================================================
	AddTag(GameplayTags.Ability_Cosmetic_SteamParticle, "Ability.Cosmetic.SteamParticle", "Cosmetic steam particle ability");

	// =========================================================
	// State (기존)
	// =========================================================
	AddTag(GameplayTags.State_Activated_SteamParticle, "State.Activated.SteamParticle", "Steam particle state");
	AddTag(GameplayTags.State_IsJumping, "State.IsJumping", "Character is jumping");
	AddTag(GameplayTags.State_IsAttacking, "State.IsAttacking", "Character is attacking");
	AddTag(GameplayTags.State_IsDead, "State.IsDead", "Character is dead");
	AddTag(GameplayTags.State_Invincible, "State.Invincible", "Character is invincible");
	AddTag(GameplayTags.State_IsSkilling, "State.IsSkilling", "Character is skilling");
	AddTag(GameplayTags.State_Skill_Cooldown, "State.Skill.Cooldown", "Skill cooldown state");

	// =========================================================
	// Data (기존)
	// =========================================================
	AddTag(GameplayTags.Data_Damage, "Data.Damage", "SetByCaller / Damage meta value");

	// =========================================================
	// Event (기존)
	// =========================================================
	AddTag(GameplayTags.Event_Equip_Weapon, "Event.Equip.Weapon", "Equip weapon event");
	AddTag(GameplayTags.Event_Unequip_Weapon, "Event.Unequip.Weapon", "Unequip weapon event");
	AddTag(GameplayTags.Event_Skill_CheckHit, "Event.Skill.CheckHit", "Skill check hit event");

	// =========================================================
	// GameplayCue (기존)
	// =========================================================
	AddTag(GameplayTags.GameplayCue_Attack_Hit, "GameplayCue.Attack.Hit", "Attack hit cue");
	AddTag(GameplayTags.GameplayCue_Box_Open, "GameplayCue.Box.Open", "Box open cue");
	AddTag(GameplayTags.GameplayCue_Damage, "GameplayCue.Damage", "Damage cue");

	// =========================================================
	// InputTag (기존: InputTag.* 네임스페이스)
	// =========================================================
	AddTag(GameplayTags.InputTag_Native_Move, "InputTag.Native.Move", "Native move input");
	AddTag(GameplayTags.InputTag_Native_Look, "InputTag.Native.Look", "Native look input");
	AddTag(GameplayTags.InputTag_Native_Interaction, "InputTag.Native.Interaction", "Native interaction input");
	AddTag(GameplayTags.InputTag_Native_InventoryToggle, "InputTag.Native.InventoryToggle", "Inventory Toggle input");

	// Ability Input Tags (기존)
	AddTag(GameplayTags.InputTag_Ability_Jump, "InputTag.Ability.Jump", "Ability input: Jump");
	AddTag(GameplayTags.InputTag_Ability_SuperJump, "InputTag.Ability.SuperJump", "Ability input: SuperJump");
	AddTag(GameplayTags.InputTag_Ability_Attack, "InputTag.Ability.Attack", "Ability input: Attack");
	AddTag(GameplayTags.InputTag_Ability_Skill, "InputTag.Ability.Skill", "Ability input: Skill");

	// =========================================================
	// Slot.* (장착 슬롯)
	// =========================================================
	AddTag(GameplayTags.Slot_Weapon_Main, "Slot.Weapon.Main", "Equipment slot: Main weapon");
	AddTag(GameplayTags.Slot_Weapon_Sub, "Slot.Weapon.Sub", "Equipment slot: Sub weapon (optional)");
	AddTag(GameplayTags.Slot_Armor_Head, "Slot.Armor.Head", "Equipment slot: Head armor (optional)");
	AddTag(GameplayTags.Slot_Armor_Chest, "Slot.Armor.Chest", "Equipment slot: Chest armor (optional)");

	// =========================================================
	// Item.* (아이템 분류/인벤 필터/드랍)
	// =========================================================
	AddTag(GameplayTags.Item_Weapon_Sword, "Item.Weapon.Sword", "Item category: Sword weapon");
	AddTag(GameplayTags.Item_Weapon_Staff, "Item.Weapon.Staff", "Item category: Staff weapon");
	AddTag(GameplayTags.Item_Consumable, "Item.Consumable", "Item category: Consumable (optional)");

	// =========================================================
	// Weapon.* (무기 타입/애니 분기 기준)
	// =========================================================
	AddTag(GameplayTags.Weapon_Sword_OneHand, "Weapon.Sword.OneHand", "Weapon type: One-hand sword");
	AddTag(GameplayTags.Weapon_Staff_OneHand, "Weapon.Staff.OneHand", "Weapon type: One-hand staff");
	AddTag(GameplayTags.Weapon_Staff_TwoHand, "Weapon.Staff.TwoHand", "Weapon type: Two-hand staff");

	// =========================================================
	// State.AnimStyle.* (ASC LooseTags로 ABP가 읽는 스타일)
	// =========================================================
	AddTag(GameplayTags.State_AnimStyle_Unarmed, "State.AnimStyle.Unarmed", "Anim style: Unarmed");
	AddTag(GameplayTags.State_AnimStyle_Sword1H, "State.AnimStyle.Sword1H", "Anim style: Sword 1H");
	AddTag(GameplayTags.State_AnimStyle_Staff2H, "State.AnimStyle.Staff2H", "Anim style: Staff 2H");

	// =========================================================
	// Input.* (EnhancedInput → AbilityTag 매핑 키)
	// =========================================================
	AddTag(GameplayTags.Input_Attack_Light, "Input.Attack.Light", "Input key: Light attack");
	AddTag(GameplayTags.Input_Attack_Heavy, "Input.Attack.Heavy", "Input key: Heavy attack");
	AddTag(GameplayTags.Input_Skill_Primary, "Input.Skill.Primary", "Input key: Primary skill");
	AddTag(GameplayTags.Input_Skill_Secondary, "Input.Skill.Secondary", "Input key: Secondary skill");
	AddTag(GameplayTags.Input_Dodge, "Input.Dodge", "Input key: Dodge");
	AddTag(GameplayTags.Input_Block, "Input.Block", "Input key: Block");
}
