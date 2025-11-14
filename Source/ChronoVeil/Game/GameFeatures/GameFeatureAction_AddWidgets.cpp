//// Source/ChronoVeil/Game/GameFeatures/GameFeatureAction_AddWidgets.cpp
//
//#include "Game/GameFeatures/GameFeatureAction_AddWidgets.h"
//#include "CommonUIExtensions.h"
//#include "Components/GameFrameworkComponentManager.h"
//#include "Game/UI/CVHUD.h"
//#include "Engine/World.h"
//#include "GameFramework/PlayerController.h"
//
//void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
//{
//	ACVHUD* HUD = CastChecked<ACVHUD>(Actor);
//
//	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player))
//	{
//		for (const FCVHUDLayoutRequest& Entry : Layout)
//		{
//			if (TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Entry.LayoutClass.Get())
//			{
//				ActiveData.LayoutsAdded.Add(
//					UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
//			}
//		}
//
//		UUIExtensionSubsystem* ExtensionSubsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();
//		for (const FCVHUDElementEntry& Entry : Widgets)
//		{
//			ActiveData.ExtensionHandles.Add(
//				ExtensionSubsystem->RegisterExtensionAsWidgetForContext(
//					Entry.SlotID,
//					LocalPlayer,
//					Entry.WidgetClass.Get(),
//					-1));
//		}
//	}
//}
//
//void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
//{
//	ACVHUD* HUD = CastChecked<ACVHUD>(Actor);
//
//	for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActiveData.LayoutsAdded)
//	{
//		if (AddedLayout.IsValid())
//		{
//			AddedLayout->DeactivateWidget();
//		}
//	}
//	ActiveData.LayoutsAdded.Reset();
//
//	for (FUIExtensionHandle& Handle : ActiveData.ExtensionHandles)
//	{
//		Handle.Unregister();
//	}
//	ActiveData.ExtensionHandles.Reset();
//}
//
//void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
//{
//	Super::OnGameFeatureDeactivating(Context);
//}
//
//void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
//{
//	UWorld* World = WorldContext.World();
//	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
//	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);
//
//	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
//	{
//		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
//		{
//			TSoftClassPtr<AActor> HUDActorClass = ACVHUD::StaticClass();
//
//			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
//				HUDActorClass,
//				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
//					this, &ThisClass::HandleActorExtension, ChangeContext));
//
//			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
//		}
//	}
//}
//
//void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
//{
//	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);
//
//	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
//		(EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
//	{
//		RemoveWidgets(Actor, ActiveData);
//	}
//	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) ||
//		(EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
//	{
//		AddWidgets(Actor, ActiveData);
//	}
//}
