//#include "RSElectricFieldPhaseData.h"
//
//#include "System/LyraAssetManager.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSElectricFieldPhaseData)
//
//const URSElectricFieldPhaseData& URSElectricFieldPhaseData::Get()
//{
//	return ULyraAssetManager::Get().GetElectricFieldPhaseData();
//}
//
//bool URSElectricFieldPhaseData::IsValidPhaseIndex(int32 PhaseIndex) const
//{
//	return PhaseEntries.IsValidIndex(PhaseIndex);
//}
//
//const FRSElectricFieldPhaseEntry& URSElectricFieldPhaseData::GetPhaseEntry(int32 PhaseIndex) const
//{
//	if (IsValidPhaseIndex(PhaseIndex) == false)
//	{
//		static FRSElectricFieldPhaseEntry EmptyEntry;
//		return EmptyEntry;
//	}
//
//	return PhaseEntries[PhaseIndex];
//}
