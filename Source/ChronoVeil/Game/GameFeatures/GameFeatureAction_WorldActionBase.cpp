//
//#include "Game/GameFeatures/GameFeatureAction_WorldActionBase.h"
//#include "Engine/Engine.h"
//#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_WorldActionBase)
//
//void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
//{
//	// 모든 WorldContext를 순회
//	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
//	{
//		// 해당 GameFeature가 이 월드에 적용되어야 하는지 확인
//		if (Context.ShouldApplyToWorldContext(WorldContext))
//		{
//			// 파생 클래스에서 구현하는 AddToWorld 호출
//			AddToWorld(WorldContext, Context);
//		}
//	}
//}
