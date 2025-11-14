//// Source/ChronoVeil/Game/GameFeatures/GameFeatureAction_WorldActionBase.h
//
//#pragma once
//
//#include "Containers/Map.h"
//#include "GameFeatureAction.h"
//#include "GameFeaturesSubsystem.h"
//#include "GameFeatureAction_WorldActionBase.generated.h"
//
//class FDelegateHandle;
//class UGameInstance;
//struct FGameFeatureActivatingContext;
//struct FGameFeatureDeactivatingContext;
//struct FWorldContext;
//
///**
// * 월드를 순회하면서 GameFeature를 적용하는 베이스 액션
// */
//UCLASS(Abstract)
//class CHRONOVEIL_API UGameFeatureAction_WorldActionBase : public UGameFeatureAction
//{
//	GENERATED_BODY()
//
//public:
//	/** UGameFeatureAction interface */
//	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
//
//	/** 파생 클래스에서 구현해야 하는 함수: 월드에 실제로 적용 */
//	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld, );
//};
