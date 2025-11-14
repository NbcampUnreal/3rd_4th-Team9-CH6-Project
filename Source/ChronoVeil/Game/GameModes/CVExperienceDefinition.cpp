#include "Game/GameModes/CVExperienceDefinition.h"   // 실제 경로와 일치
#include "Engine/AssetManager.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/ConstructorHelpers.h"
#include "Game/Character/CVPawnData.h"                               // UCVPawnData 정의 포함(경로는 실제 위치로)

UCVExperienceDefinition::UCVExperienceDefinition(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 필요 시 기본값 세팅
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
        const_cast<TSoftObjectPtr<UCVPawnData>&>(DefaultPawnData).LoadSynchronous(); // 에디터/요청시 로드
        return DefaultPawnData.Get();
    }
    return nullptr;
}

