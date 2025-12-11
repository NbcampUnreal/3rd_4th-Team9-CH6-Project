//#pragma once
//
//#include "RSElectricFieldPhaseData.generated.h"
//
//USTRUCT(BlueprintType)
//struct FRSElectricFieldPhaseEntry
//{
//	GENERATED_BODY()
//	
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float TargetRadius = 0.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float BreakTime = 0.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float NoticeTime = 0.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float ShrinkTime = 0.f;
//};
//
//UCLASS(BlueprintType, Const)
//class URSElectricFieldPhaseData : public UPrimaryDataAsset
//{
//	GENERATED_BODY()
//
//public:
//	static const URSElectricFieldPhaseData& Get();
//
//public:
//	bool IsValidPhaseIndex(int32 PhaseIndex) const;
//	const FRSElectricFieldPhaseEntry& GetPhaseEntry(int32 PhaseIndex) const;
//	
//private:
//	UPROPERTY(EditDefaultsOnly)
//	TArray<FRSElectricFieldPhaseEntry> PhaseEntries;
//};
