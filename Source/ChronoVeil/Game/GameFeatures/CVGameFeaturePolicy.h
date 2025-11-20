//#pragma once
//
//#include "Containers/Array.h"
//#include "GameFeatureStateChangeObserver.h"
//#include "GameFeaturesProjectPolicies.h"
//#include "UObject/Object.h"
//#include "UObject/ObjectPtr.h"
//#include "UObject/UObjectGlobals.h"
//#include "CVGameFeaturePolicy.generated.h"
//
//class UGameFeatureData;
//
///**
// * ChronoVeil의 GameplayFeaturePolicy
// * - GameFeature Plugin의 Register/Activate 상태 모니터링
// */
//UCLASS()
//class CHRONOVEIL_API UCVGameplayFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
//{
//	GENERATED_BODY()
//
//public:
//	UCVGameplayFeaturePolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/** GameFeaturesProjectPolicies interface */
//	virtual void InitGameFeatureManager() override;
//	virtual void ShutdownGameFeatureManager() override;
//
//	/** Observer로 등록한 객체를 관리 */
//	UPROPERTY(Transient)
//	TArray<TObjectPtr<UObject>> Observers;
//};
//
///**
// * GameFeature Plugin의 Register/Unregister 단계에서 GameplayCuePath를 등록/해제
// */
//UCLASS()
//class CHRONOVEIL_API UCVGameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
//{
//	GENERATED_BODY()
//
//public:
//	/** IGameFeatureStateChangeObserver interface */
//	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
//	virtual void OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
//};
