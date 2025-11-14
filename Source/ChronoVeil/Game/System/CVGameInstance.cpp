// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/System/CVGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Game/CVGameplayTags.h"

void UCVGameInstance::Init()
{
	Super::Init();

	if (UGameFrameworkComponentManager* Manager = GetSubsystem<UGameFrameworkComponentManager>(this))
	{
		const FCVGameplayTags& Tags = FCVGameplayTags::Get();

		// 선형 InitState 사슬 등록
		Manager->RegisterInitState(Tags.InitState_Spawned, false, FGameplayTag());
		Manager->RegisterInitState(Tags.InitState_DataAvailable, false, Tags.InitState_Spawned);
		Manager->RegisterInitState(Tags.InitState_DataInitialized, false, Tags.InitState_DataAvailable);
		Manager->RegisterInitState(Tags.InitState_GameplayReady, false, Tags.InitState_DataInitialized);
	}
}

void UCVGameInstance::Shutdown()
{
	Super::Shutdown();
}
