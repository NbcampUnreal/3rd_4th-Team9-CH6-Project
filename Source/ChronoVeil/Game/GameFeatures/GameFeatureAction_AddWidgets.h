//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "CommonActivatableWidget.h"
//#include "Blueprint/UserWidget.h"
//#include "GameplayTagContainer.h"
//#include "UIExtensionSystem.h"
//#include "Game/GameFeatures/GameFeatureAction_WorldActionBase.h"
//#include "GameFeatureAction_AddWidgets.generated.h"
//
///** forward declarations */
//struct FComponentRequestHandle;
//
///** HUD의 Layout 요청 (CV 버전) */
//USTRUCT()
//struct FCVHUDLayoutRequest
//{
//	GENERATED_BODY()
//
//	/** UI 레이아웃으로 사용할 CommonActivatableWidget */
//	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
//	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;
//
//	/** PrimaryGameLayout 의 LayerID */
//	UPROPERTY(EditAnywhere, Category = UI)
//	FGameplayTag LayerID;
//};
//
//USTRUCT()
//struct FCVHUDElementEntry
//{
//	GENERATED_BODY()
//
//	/** CVHUDLayout 위에 올릴 대상이 되는 Widget Class */
//	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
//	TSoftClassPtr<UUserWidget> WidgetClass;
//
//	/** SlotID는 CVHUDLayoutRequest 에서 정의된 Slot(GameplayTag) */
//	UPROPERTY(EditAnywhere, Category = UI)
//	FGameplayTag SlotID;
//};
//
//UCLASS()
//class CHRONOVEIL_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
//{
//	GENERATED_BODY()
//
//public:
//	struct FPerContextData
//	{
//		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
//		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
//
//		/** Lyra와 동일하게, HUDElement는 UIExtension으로 관리 */
//		TArray<FUIExtensionHandle> ExtensionHandles;
//	};
//
//	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
//	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);
//
//	/** UGameFeatureAction interface */
//	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
//
//	/** UGameFeatureAction_WorldActionBase interface */
//	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
//
//	/** GameFrameworkComponentManager의 Extension 이벤트 처리 */
//	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
//
//	/** GFA Add/Remove 상태 관리 */
//	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
//
//	/** 레이아웃 정의 목록 */
//	UPROPERTY(EditAnywhere, Category = UI)
//	TArray<FCVHUDLayoutRequest> Layout;
//
//	/** Layout 위에 올릴 Widget 목록 */
//	UPROPERTY(EditAnywhere, Category = UI)
//	TArray<FCVHUDElementEntry> Widgets;
//};
