// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#define ABILITY_COSMETIC_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("Ability.Cosmetic.SteamParticle"))

#define STATE_ACTIVATED_STEAMPARTICLE FGameplayTag::RequestGameplayTag(FName("State.Activated.SteamParticle"))

/**
 * 
 */
class REMNANTSOUL_API RSGameplayTags
{
public:
	RSGameplayTags();
	~RSGameplayTags();
};
