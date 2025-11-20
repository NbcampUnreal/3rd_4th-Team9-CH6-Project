// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FCVGameplayTags
{
//public:
//	CVGameplayTags();
//	~CVGameplayTags();

public:
	static const FCVGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// GameFrameworkComponentManager InitState
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

	// Enhanced Input 기본 태그
	FGameplayTag InputTag_Native_Move;
	FGameplayTag InputTag_Native_Look_Mouse;
	FGameplayTag InputTag_Native_Look_Aim;
	FGameplayTag InputTag_Native_Crouch;
	// TODO: 필요 시 추가해야함. : InputTag_Weapon_Fire / Reload / ADS, Input.Layer. 등등..

	FGameplayTag Ability_InputBlocked;

	FGameplayTag Cooldown_Obstacle;

private:
	static FCVGameplayTags GameplayTags;
	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	static void AddAllTags(class UGameplayTagsManager& Manager);
};
