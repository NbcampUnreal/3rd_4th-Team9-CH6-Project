#include "RSGameplayTags.h"
#include "GameplayTagsManager.h"

FRSGameplayTags FRSGameplayTags::GameplayTags;

void FRSGameplayTags::InitializeNativeTags()
{
	static bool bInitialized = false;
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;

	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	AddAllTags(Manager);
}

void FRSGameplayTags::AddTag(UGameplayTagsManager& Manager, FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = Manager.AddNativeGameplayTag(
		FName(TagName),
		FString(TEXT("(Native) ")) + FString(TagComment)
	);
}

void FRSGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// Ability
	AddTag(Manager, GameplayTags.Ability_Cosmetic_SteamParticle, "Ability.Cosmetic.SteamParticle", "Cosmetic steam particle ability");

	// State
	AddTag(Manager, GameplayTags.State_Activated_SteamParticle, "State.Activated.SteamParticle", "Steam particle state");
	AddTag(Manager, GameplayTags.State_IsJumping, "State.IsJumping", "Character is jumping");
	AddTag(Manager, GameplayTags.State_IsAttacking, "State.IsAttacking", "Character is attacking");
	AddTag(Manager, GameplayTags.State_IsDead, "State.IsDead", "Character is dead");
	AddTag(Manager, GameplayTags.State_Invincible, "State.Invincible", "Character is invincible");
	AddTag(Manager, GameplayTags.State_IsSkilling, "State.IsSkilling", "Character is skilling");
	AddTag(Manager, GameplayTags.State_Skill_Cooldown, "State.Skill.Cooldown", "Skill cooldown state");

	// Data
	AddTag(Manager, GameplayTags.Data_Damage, "Data.Damage", "SetByCaller / Damage meta value");

	// Event
	AddTag(Manager, GameplayTags.Event_Equip_Weapon, "Event.Equip.Weapon", "Equip weapon event");
	AddTag(Manager, GameplayTags.Event_Unequip_Weapon, "Event.Unequip.Weapon", "Unequip weapon event");
	AddTag(Manager, GameplayTags.Event_Skill_CheckHit, "Event.Skill.CheckHit", "Skill check hit event");

	// GameplayCue
	AddTag(Manager, GameplayTags.GameplayCue_Attack_Hit, "GameplayCue.Attack.Hit", "Attack hit cue");
	AddTag(Manager, GameplayTags.GameplayCue_Box_Open, "GameplayCue.Box.Open", "Box open cue");
	AddTag(Manager, GameplayTags.GameplayCue_Damage, "GameplayCue.Damage", "Damage cue");

	// InputTag.*
	AddTag(Manager, GameplayTags.InputTag_Native_Move, "InputTag.Native.Move", "Native move input");
	AddTag(Manager, GameplayTags.InputTag_Native_Look, "InputTag.Native.Look", "Native look input");
	AddTag(Manager, GameplayTags.InputTag_Native_Interaction, "InputTag.Native.Interaction", "Native interaction input");
	AddTag(Manager, GameplayTags.InputTag_Native_InventoryToggle, "InputTag.Native.InventoryToggle", "Inventory Toggle input");

	AddTag(Manager, GameplayTags.InputTag_Ability_Jump, "InputTag.Ability.Jump", "Ability input: Jump");
	AddTag(Manager, GameplayTags.InputTag_Ability_SuperJump, "InputTag.Ability.SuperJump", "Ability input: SuperJump");
	AddTag(Manager, GameplayTags.InputTag_Ability_Attack, "InputTag.Ability.Attack", "Ability input: Attack");
	AddTag(Manager, GameplayTags.InputTag_Ability_Skill, "InputTag.Ability.Skill", "Ability input: Skill");

	// Slot.*
	AddTag(Manager, GameplayTags.Slot_Weapon_Main, "Slot.Weapon.Main", "Equipment slot: Main weapon");
	AddTag(Manager, GameplayTags.Slot_Weapon_Sub, "Slot.Weapon.Sub", "Equipment slot: Sub weapon (optional)");
	AddTag(Manager, GameplayTags.Slot_Armor_Head, "Slot.Armor.Head", "Equipment slot: Head armor (optional)");
	AddTag(Manager, GameplayTags.Slot_Armor_Chest, "Slot.Armor.Chest", "Equipment slot: Chest armor (optional)");

	// Item.*
	AddTag(Manager, GameplayTags.Item_Weapon_Sword, "Item.Weapon.Sword", "Item category: Sword weapon");
	AddTag(Manager, GameplayTags.Item_Weapon_Staff, "Item.Weapon.Staff", "Item category: Staff weapon");
	AddTag(Manager, GameplayTags.Item_Consumable, "Item.Consumable", "Item category: Consumable (optional)");

	// Weapon.*
	AddTag(Manager, GameplayTags.Weapon_Sword_OneHand, "Weapon.Sword.OneHand", "Weapon type: One-hand sword");
	AddTag(Manager, GameplayTags.Weapon_Staff_OneHand, "Weapon.Staff.OneHand", "Weapon type: One-hand staff");
	AddTag(Manager, GameplayTags.Weapon_Staff_TwoHand, "Weapon.Staff.TwoHand", "Weapon type: Two-hand staff");

	// State.AnimStyle.*
	AddTag(Manager, GameplayTags.State_AnimStyle_Unarmed, "State.AnimStyle.Unarmed", "Anim style: Unarmed");
	AddTag(Manager, GameplayTags.State_AnimStyle_Sword1H, "State.AnimStyle.Sword1H", "Anim style: Sword 1H");
	AddTag(Manager, GameplayTags.State_AnimStyle_Staff2H, "State.AnimStyle.Staff2H", "Anim style: Staff 2H");

	// Input.*
	AddTag(Manager, GameplayTags.Input_Attack_Light, "Input.Attack.Light", "Input key: Light attack");
	AddTag(Manager, GameplayTags.Input_Attack_Heavy, "Input.Attack.Heavy", "Input key: Heavy attack");
	AddTag(Manager, GameplayTags.Input_Skill_Primary, "Input.Skill.Primary", "Input key: Primary skill");
	AddTag(Manager, GameplayTags.Input_Skill_Secondary, "Input.Skill.Secondary", "Input key: Secondary skill");
	AddTag(Manager, GameplayTags.Input_Dodge, "Input.Dodge", "Input key: Dodge");
	AddTag(Manager, GameplayTags.Input_Block, "Input.Block", "Input key: Block");
}
