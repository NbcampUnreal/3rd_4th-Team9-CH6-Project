//// Source/ChronoVeil/Game/GameFeatures/CVGameplayFeaturePolicy.cpp
//
//#include "Game/GameFeatures/CVGameplayFeaturePolicy.h"
//#include "GameFeatureAction.h"
//#include "GameFeatureAction_AddGameplayCuePath.h"
//#include "GameFeatureData.h"
//#include "GameplayCueSet.h"
//#include "Game/AbilitySystem/CVGameplayCueManager.h"
//#include "GameFeaturesSubsystem.h"
//
//UCVGameplayFeaturePolicy::UCVGameplayFeaturePolicy(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}
//
//void UCVGameplayFeaturePolicy::InitGameFeatureManager()
//{
//	// GameplayCuePaths Observer µî·Ï
//	Observers.Add(NewObject<UCVGameFeature_AddGameplayCuePaths>());
//
//	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
//	for (UObject* Observer : Observers)
//	{
//		Subsystem.AddObserver(Observer);
//	}
//
//	Super::InitGameFeatureManager();
//}
//
//void UCVGameplayFeaturePolicy::ShutdownGameFeatureManager()
//{
//	Super::ShutdownGameFeatureManager();
//
//	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
//	for (UObject* Observer : Observers)
//	{
//		Subsystem.RemoveObserver(Observer);
//	}
//
//	Observers.Empty();
//}
//
//void UCVGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	const FString PluginRootPath = TEXT("/") + PluginName;
//
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
//
//			if (UCVGameplayCueManager* GCM = UCVGameplayCueManager::Get())
//			{
//				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
//				const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//
//					GCM->AddGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets=*/false);
//				}
//
//				if (!DirsToAdd.IsEmpty())
//				{
//					GCM->InitializeRuntimeObjectLibrary();
//				}
//
//				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//				if (PreInitializeNumCues != PostInitializeNumCues)
//				{
//					GCM->RefreshGameplayCuePrimaryAsset();
//				}
//			}
//		}
//	}
//}
//
//void UCVGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	const FString PluginRootPath = TEXT("/") + PluginName;
//
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
//
//			if (UCVGameplayCueManager* GCM = UCVGameplayCueManager::Get())
//			{
//				int32 NumRemoved = 0;
//
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, /*bShouldRescanCueAssets=*/false);
//				}
//
//				ensure(NumRemoved == DirsToAdd.Num());
//
//				if (NumRemoved > 0)
//				{
//					GCM->InitializeRuntimeObjectLibrary();
//				}
//			}
//		}
//	}
//}
