// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/System/CVAssetManager.h"
#include "Game/CVGameplayTags.h"
#include "Engine/Engine.h"

UCVAssetManager::UCVAssetManager() {}

UCVAssetManager& UCVAssetManager::Get()
{
	check(GEngine);
	if (UCVAssetManager* Singleton = Cast<UCVAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	// 설정 누락 시 즉시 알림
	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManagerClassName. Set [/Script/ChronoVeil.CVAssetManager] in DefaultEngine.ini"));
	// Unreachable, but keep compiler happy
	return *NewObject<UCVAssetManager>();
}

PRAGMA_DISABLE_OPTIMIZATION
void UCVAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 네이티브 태그 등록
	FCVGameplayTags::InitializeNativeTags();
}
PRAGMA_ENABLE_OPTIMIZATION

bool UCVAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* UCVAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	TUniquePtr<FScopeLogTime> ScopeLog;
	if (ShouldLogAssetLoads())
	{
		ScopeLog = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
	}

	if (UAssetManager::IsValid())
	{
		return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
	}

	return AssetPath.TryLoad();
}

void UCVAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
