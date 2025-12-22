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
	// ---- State / Data / Event / Cue ----
	AddTag(GameplayTags.State_IsJumping, "State.IsJumping", "Character is jumping");
	AddTag(GameplayTags.State_IsAttacking, "State.IsAttacking", "Character is attacking");
	AddTag(GameplayTags.State_Invincible, "State.Invincible", "Character is invincible");

	AddTag(GameplayTags.Data_Damage, "Data.Damage", "SetByCaller / Damage meta value");

	AddTag(GameplayTags.Event_Equip_Weapon, "Event.Equip.Weapon", "Equip weapon event");
	AddTag(GameplayTags.Event_Unequip_Weapon, "Event.Unequip.Weapon", "Unequip weapon event");

	AddTag(GameplayTags.GameplayCue_Attack_Hit, "GameplayCue.Attack.Hit", "Attack hit cue");

	// ---- Input Tags (Native) ----
	AddTag(GameplayTags.InputTag_Native_Move, "InputTag.Native.Move", "Native move input");
	AddTag(GameplayTags.InputTag_Native_Look, "InputTag.Native.Look", "Native look input");

	// ---- Input Tags (Ability) ----
	AddTag(GameplayTags.InputTag_Ability_Jump, "InputTag.Ability.Jump", "Ability input: Jump");
	AddTag(GameplayTags.InputTag_Ability_Attack, "InputTag.Ability.Attack", "Ability input: Attack");
	AddTag(GameplayTags.InputTag_Ability_Skill, "InputTag.Ability.Skill", "Ability input: Skill");
}
