#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#define ABILITY_COSMETIC_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("Ability.Cosmetic.SteamParticle"))

#define STATE_ACTIVATED_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("State.Activated.SteamParticle"))

#define STATE_ISJUMPING FGameplayTag::RequestGameplayTag(FName("State.IsJumping"))

#define STATE_ISATTACKING FGameplayTag::RequestGameplayTag(FName("State.IsAttacking"))

#define DATA_DAMAGE FGameplayTag::RequestGameplayTag(FName("Data.Damage"))

#define STATE_ISDEAD FGameplayTag::RequestGameplayTag(FName("State.IsDead"))

#define STATE_INVINCIBLE FGameplayTag::RequestGameplayTag(FName("State.Invincible"))

#define GAMEPLAYCUE_ATTACK_HIT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Attack.Hit"))

#define GAMEPLAYCUE_BOX_OPEN FGameplayTag::RequestGameplayTag(FName("GameplayCue.Box.Open"))

#define GAMEPLAYCUE_DAMAGE FGameplayTag::RequestGameplayTag(FName("GameplayCue.Damage"))


#define EVENT_EQUIP_WEAPON FGameplayTag::RequestGameplayTag(FName("Event.Equip.Weapon"))
#define EVENT_UNEQUIP_WEAPON FGameplayTag::RequestGameplayTag(FName("Event.Unequip.Weapon"))

#define STATE_ISSKILLING FGameplayTag::RequestGameplayTag(FName("State.IsSkilling"))

#define EVENT_SKILL_CHECKHIT FGameplayTag::RequestGameplayTag(FName("Event.Skill.CheckHit"))

#define STATE_SKILL_COOLDOWN FGameplayTag::RequestGameplayTag(FName("State.Skill.Cooldown"))


/* InputAction Tag들 */
//#define INPUT_ATTACK FGameplayTag::RequestGameplayTag(TEXT("Input.Attack"))
//#define INPUT_SKILL  FGameplayTag::RequestGameplayTag(TEXT("Input.Skill"))
//#define INPUT_JUMP   FGameplayTag::RequestGameplayTag(TEXT("Input.Jump"))
//#define INPUT_SUPERJUMP   FGameplayTag::RequestGameplayTag(TEXT("Input.SuperJump"))


class REMNANTSOUL_API RSGameplayTags
{
public:
	RSGameplayTags();
	~RSGameplayTags();
};
