//// Source/ChronoVeil/Game/GameFeatures/GameFeatureAction_AddInputConfig.h
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Game/GameFeatures/GameFeatureAction_WorldActionBase.h"
//#include "Game/Input/CVMappableConfigPair.h"
//#include "GameFeatureAction_AddInputConfig.generated.h"
//
///** forward declarations */
//struct FComponentRequestHandle;
//class APawn;
//
///**
// * GameFeature 활성화 시 플레이어의 EnhancedInput에 InputConfig를 추가하는 액션
// */
//UCLASS()
//class CHRONOVEIL_API UGameFeatureAction_AddInputConfig : public UGameFeatureAction_WorldActionBase
//{
//	GENERATED_BODY()
//
//public:
//	/** UGameFeatureAction interface */
//	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
//	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
//
//	/** UGameFeatureAction_WorldActionBase interface */
//	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
//
//private:
//	struct FPerContextData
//	{
//		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
//		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
//	};
//
//	/** GameFrameworkComponentManager에서 Extension 이벤트가 왔을 때 처리 */
//	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
//
//	/** InputConfig를 EnhancedInputLocalPlayerSubsystem에 추가/제거 */
//	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
//	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);
//
//	/** Per-context 데이터 초기화 */
//	void Reset(FPerContextData& ActiveData);
//
//	/** world -> (extension request, pawn) */
//	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
//
//	/** PlayerMappableInputConfig를 들고 있는 멤버 변수 (CV 버전) */
//	UPROPERTY(EditAnywhere, Category = "Input")
//	TArray<FCVMappableConfigPair> InputConfigs;
//};
