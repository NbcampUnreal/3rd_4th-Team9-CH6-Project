#include "RSClassData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSClassData)

const URSClassData& URSClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}

const FRSClassInfoEntry& URSClassData::GetClassInfoEntry(ECharacterClassType ClassType) const
{
	const int32 ClassIndex = (int32)ClassType;
	if (ClassIndex < 0 || ClassIndex >= (int32)ECharacterClassType::Count)
	{
		static FRSClassInfoEntry EmptyEntry;
		return EmptyEntry;
	}
	
	return ClassInfoEntries[ClassIndex];
}
