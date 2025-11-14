// CVExperienceManagerComponent.cpp
#include "Game/GameModes/CVExperienceManagerComponent.h"
#include "Game/GameModes/CVExperienceDefinition.h"
#include "Engine/AssetManager.h"

void UCVExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnCVExperienceLoaded::FDelegate&& Delegate)
{
    if (IsExperienceLoaded())
    {
        Delegate.Execute(CurrentExperience);
    }
    else
    {
        OnExperienceLoaded.Add(MoveTemp(Delegate));
    }
}

//1.
//void UCVExperienceManagerComponent::ServerSetCurrentExperience(const FPrimaryAssetId& ExperienceId)
//{
//    check(GetOwner() && GetOwner()->HasAuthority());
//    check(LoadState == ECVExperienceLoadState::Unloaded);
//
//    // CDO 로드 (경량)
//    TSubclassOf<UCVExperienceDefinition> AssetClass;
//    {
//        FSoftObjectPath Path = UAssetManager::Get().GetPrimaryAssetPath(ExperienceId);
//        AssetClass = Cast<UClass>(Path.TryLoad());
//    }
//
//    const UCVExperienceDefinition* ExperienceCDO = GetDefault<UCVExperienceDefinition>(AssetClass);
//    check(ExperienceCDO);
//
//    CurrentExperience = ExperienceCDO;
//    StartExperienceLoad();
//}

PRAGMA_DISABLE_OPTIMIZATION
void UCVExperienceManagerComponent::ServerSetCurrentExperience(const FPrimaryAssetId& ExperienceId)
{
    check(GetOwner() && GetOwner()->HasAuthority());
    check(LoadState == ECVExperienceLoadState::Unloaded);

    const FSoftObjectPath Path = UAssetManager::Get().GetPrimaryAssetPath(ExperienceId);
    if (!Path.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[CV] PrimaryAssetPath invalid for %s"), *ExperienceId.ToString());
        return;
    }

    // 인스턴스로 로드
    UCVExperienceDefinition* ExperienceAsset = Cast<UCVExperienceDefinition>(Path.TryLoad());
    if (!ExperienceAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("[CV] Failed to load Experience asset %s (type must be CVExperienceDefinition)"),
            *ExperienceId.ToString());
        return;
    }

    CurrentExperience = ExperienceAsset;      // 그대로 보관(인스턴스)
    StartExperienceLoad();                    // 경량이면 즉시 완료
}
PRAGMA_ENABLE_OPTIMIZATION


PRAGMA_DISABLE_OPTIMIZATION
void UCVExperienceManagerComponent::StartExperienceLoad()
{
    LoadState = ECVExperienceLoadState::Loading;

    // 경량 모드: 별도 번들/플러그인 없음 → 즉시 완료 처리
    OnExperienceLoadComplete();
}
PRAGMA_ENABLE_OPTIMIZATION


void UCVExperienceManagerComponent::OnExperienceLoadComplete()
{
    LoadState = ECVExperienceLoadState::Loaded;
    OnExperienceLoaded.Broadcast(CurrentExperience);
    OnExperienceLoaded.Clear();
}

const UCVExperienceDefinition* UCVExperienceManagerComponent::GetCurrentExperienceChecked() const
{
    check(IsExperienceLoaded());
    return CurrentExperience;
}
