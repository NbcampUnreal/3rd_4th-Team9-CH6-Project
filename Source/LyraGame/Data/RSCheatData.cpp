#include "RSCheatData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCheatData)

const URSCheatData& URSCheatData::Get()
{
	return ULyraAssetManager::Get().GetCheatData();
}
