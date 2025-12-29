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
	// Ability
	AddTag(GameplayTags.Ability_Cosmetic_SteamParticle, "Ability.Cosmetic.SteamParticle", "Cosmetic steam particle ability");

	// State
	AddTag(GameplayTags.State_Activated_SteamParticle, "State.Activated.SteamParticle", "Steam particle state");
	AddTag(GameplayTags.State_IsJumping, "State.IsJumping", "Character is jumping");
	AddTag(GameplayTags.State_IsAttacking, "State.IsAttacking", "Character is attacking");
	AddTag(GameplayTags.State_IsDead, "State.IsDead", "Character is dead");
	AddTag(GameplayTags.State_Invincible, "State.Invincible", "Character is invincible");
	AddTag(GameplayTags.State_IsSkilling, "State.IsSkilling", "Character is skilling");
	AddTag(GameplayTags.State_Skill_Cooldown, "State.Skill.Cooldown", "Skill cooldown state");

	// Data
	AddTag(GameplayTags.Data_Damage, "Data.Damage", "SetByCaller / Damage meta value");

	// Event
	AddTag(GameplayTags.Event_Equip_Weapon, "Event.Equip.Weapon", "Equip weapon event");
	AddTag(GameplayTags.Event_Unequip_Weapon, "Event.Unequip.Weapon", "Unequip weapon event");
	AddTag(GameplayTags.Event_Skill_CheckHit, "Event.Skill.CheckHit", "Skill check hit event");

	// GameplayCue
	AddTag(GameplayTags.GameplayCue_Attack_Hit, "GameplayCue.Attack.Hit", "Attack hit cue");
	AddTag(GameplayTags.GameplayCue_Box_Open, "GameplayCue.Box.Open", "Box open cue");
	AddTag(GameplayTags.GameplayCue_Damage, "GameplayCue.Damage", "Damage cue");

	// InputTag
	AddTag(GameplayTags.InputTag_Native_Move, "InputTag.Native.Move", "Native move input");
	AddTag(GameplayTags.InputTag_Native_Look, "InputTag.Native.Look", "Native look input");

	AddTag(GameplayTags.InputTag_Native_Interaction, "InputTag.Native.Interaction", "Native interaction input");
	AddTag(GameplayTags.InputTag_Native_InventoryToggle,"InputTag.Native.InventoryToggle", "Inventory Toggle input");
	
	// Ability Input Tags
	AddTag(GameplayTags.InputTag_Ability_Jump, "InputTag.Ability.Jump", "Ability input: Jump");
	AddTag(GameplayTags.InputTag_Ability_Attack, "InputTag.Ability.Attack", "Ability input: Attack");
	AddTag(GameplayTags.InputTag_Ability_Skill, "InputTag.Ability.Skill", "Ability input: Skill");
}

