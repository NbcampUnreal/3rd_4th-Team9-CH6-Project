//#include "RSUIData.h"
//
//#include "System/LyraAssetManager.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSUIData)
//
//const URSUIData& URSUIData::Get()
//{
//	return ULyraAssetManager::Get().GetUIData();
//}
//
//UTexture2D* URSUIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
//{
//	const int32 ItemRarityIndex = (int32)ItemRarity;
//	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
//		return nullptr;
//	
//	return RarityInfoEntries[ItemRarityIndex].EntryTexture;
//}
//
//UTexture2D* URSUIData::GetHoverRarityTexture(EItemRarity ItemRarity) const
//{
//	const int32 ItemRarityIndex = (int32)ItemRarity;
//	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
//		return nullptr;
//	
//	return RarityInfoEntries[ItemRarityIndex].HoverTexture;
//}
//
//FColor URSUIData::GetRarityColor(EItemRarity ItemRarity) const
//{
//	const int32 ItemRarityIndex = (int32)ItemRarity;
//	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
//		return FColor::Black;
//	
//	return RarityInfoEntries[ItemRarityIndex].Color;
//}
//
//const FRSUIInfo& URSUIData::GetTagUIInfo(FGameplayTag Tag) const
//{
//	const FRSUIInfo* UIInfo = TagUIInfos.Find(Tag);
//	if (UIInfo == nullptr)
//	{
//		static FRSUIInfo EmptyInfo;
//		return EmptyInfo;
//	}
//
//	return *UIInfo;
//}
