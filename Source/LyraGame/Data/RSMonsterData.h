//#pragma once
//
//#include "RSMonsterData.generated.h"
//
//class ULyraPawnData;
//class AAIController;
//
//UCLASS(BlueprintType, Const, meta=(DisplayName="RS Monster Data"))
//class URSMonsterData : public UPrimaryDataAsset
//{
//	GENERATED_BODY()
//	
//public:
//	static const URSMonsterData& Get();
//
//public:
//	ULyraPawnData* GetPawnData(TSubclassOf<AAIController> AIControllerClass) const;
//	
//private:
//	UPROPERTY(EditDefaultsOnly)
//	TMap<TSubclassOf<AAIController>, TObjectPtr<ULyraPawnData>> PawnDataMap;
//};
