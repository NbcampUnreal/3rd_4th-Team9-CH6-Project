#pragma once
#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "CVExperienceManagerComponent.generated.h"

class UCVExperienceDefinition;

enum class ECVExperienceLoadState
{
    Unloaded,
    Loading,
    Loaded,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCVExperienceLoaded, const UCVExperienceDefinition*);

UCLASS()
class CHRONOVEIL_API UCVExperienceManagerComponent : public UGameStateComponent
{
    GENERATED_BODY()
public:
    bool IsExperienceLoaded() const { return (LoadState == ECVExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

    void CallOrRegister_OnExperienceLoaded(FOnCVExperienceLoaded::FDelegate&& Delegate);
    void ServerSetCurrentExperience(const FPrimaryAssetId& ExperienceId);

    const UCVExperienceDefinition* GetCurrentExperienceChecked() const;

private:
    void StartExperienceLoad();        // 경량 : 바로 완료 처리
    void OnExperienceLoadComplete();   // 브로드캐스트

public:
    UPROPERTY()
    TObjectPtr<const UCVExperienceDefinition> CurrentExperience = nullptr;

    ECVExperienceLoadState LoadState = ECVExperienceLoadState::Unloaded;

    FOnCVExperienceLoaded OnExperienceLoaded;
};
