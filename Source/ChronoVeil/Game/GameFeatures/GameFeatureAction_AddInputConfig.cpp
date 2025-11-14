//// Source/ChronoVeil/Game/GameFeatures/GameFeatureAction_AddInputConfig.cpp
//
//#include "Game/GameFeatures/GameFeatureAction_AddInputConfig.h"
//#include "EnhancedInputSubsystems.h"
//#include "PlayerMappableInputConfig.h"
//#include "Components/GameFrameworkComponentManager.h"
//#include "Game/Character/CVHeroComponent.h"
//#include "GameFramework/PlayerController.h"
//#include "Engine/World.h"
//#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputConfig)
//
//void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
//{
//	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
//	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
//		!ensure(ActiveData.PawnsAddedTo.IsEmpty()))
//	{
//		Reset(ActiveData);
//	}
//
//	Super::OnGameFeatureActivating(Context);
//}
//
//void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
//{
//	Super::OnGameFeatureDeactivating(Context);
//
//	FPerContextData* ActiveData = ContextData.Find(Context);
//	if (ensure(ActiveData))
//	{
//		Reset(*ActiveData);
//	}
//}
//
//void UGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
//{
//	UWorld* World = WorldContext.World();
//	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
//	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);
//
//	if (GameInstance && World && World->IsGameWorld())
//	{
//		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
//		{
//			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate =
//				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);
//
//			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
//				ComponentMan->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
//
//			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
//		}
//	}
//}
//
//void UGameFeatureAction_AddInputConfig::Reset(FPerContextData& ActiveData)
//{
//	ActiveData.ExtensionRequestHandles.Empty();
//
//	// Stack처럼 위에서부터 제거
//	while (!ActiveData.PawnsAddedTo.IsEmpty())
//	{
//		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();
//		if (PawnPtr.IsValid())
//		{
//			RemoveInputConfig(PawnPtr.Get(), ActiveData);
//		}
//		else
//		{
//			ActiveData.PawnsAddedTo.Pop();
//		}
//	}
//}
//
//void UGameFeatureAction_AddInputConfig::AddInputConfig(APawn* Pawn, FPerContextData& ActiveData)
//{
//	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
//	if (ULocalPlayer* LP = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
//	{
//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
//		{
//			FModifyContextOptions Options;
//			Options.bIgnoreAllPressedKeysUntilRelease = false;
//
//			for (const FCVMappableConfigPair& Pair : InputConfigs)
//			{
//				if (Pair.bShouldActivateAutomatically)
//				{
//					Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
//				}
//			}
//
//			ActiveData.PawnsAddedTo.AddUnique(Pawn);
//		}
//	}
//}
//
//void UGameFeatureAction_AddInputConfig::RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData)
//{
//	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
//	if (ULocalPlayer* LP = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
//	{
//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
//		{
//			for (const FCVMappableConfigPair& Pair : InputConfigs)
//			{
//				Subsystem->RemovePlayerMappableConfig(Pair.Config.LoadSynchronous());
//			}
//
//			ActiveData.PawnsAddedTo.Remove(Pawn);
//		}
//	}
//}
//
//void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
//{
//	APawn* AsPawn = CastChecked<APawn>(Actor);
//	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);
//
//	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded ||
//		EventName == UCVHeroComponent::NAME_BindInputsNow)
//	{
//		AddInputConfig(AsPawn, ActiveData);
//	}
//	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved)
//	{
//		RemoveInputConfig(AsPawn, ActiveData);
//	}
//}
