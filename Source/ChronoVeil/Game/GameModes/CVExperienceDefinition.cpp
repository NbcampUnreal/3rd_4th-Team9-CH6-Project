#include "Game/GameModes/CVExperienceDefinition.h"
#include "Engine/AssetManager.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/ConstructorHelpers.h"
#include "Game/Character/CVPawnData.h"

UCVExperienceDefinition::UCVExperienceDefinition(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

FPrimaryAssetId UCVExperienceDefinition::GetPrimaryAssetId() const
{
    // PrimaryAssetType을 명시적으로 고정해서 사용하고 싶을 때
    // Content Browser의 Primary Asset Rules를 설정했다면 이 이름과 일치시켜야 함
    return FPrimaryAssetId(TEXT("CVExperienceDefinition"), GetFName());
}

const UCVPawnData* UCVExperienceDefinition::GetDefaultPawnData() const
{
    if (!DefaultPawnData.IsNull())
    {
        const_cast<TSoftObjectPtr<UCVPawnData>&>(DefaultPawnData).LoadSynchronous();
        return DefaultPawnData.Get();
    }
    return nullptr;
}

