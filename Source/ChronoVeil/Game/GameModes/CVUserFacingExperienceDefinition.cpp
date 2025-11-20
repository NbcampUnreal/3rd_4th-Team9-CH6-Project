#include "Game/GameModes/CVUserFacingExperienceDefinition.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

void UCVUserFacingExperienceDefinition::OpenInEditor() const
{
    if (!MapID.IsValid() || !ExperienceID.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[CV] UserFacingExperience not set (Map/Experience)!"));
        return;
    }

    // MapID → 패키지 경로 → 맵 이름 추출
    const FSoftObjectPath MapPath = UAssetManager::Get().GetPrimaryAssetPath(MapID);
    const FString PackageName = MapPath.GetLongPackageName();
    const FString MapName = FPackageName::GetShortName(PackageName);

    const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
    const FString Options = FString::Printf(TEXT("Experience=%s"), *ExperienceName);

    // 에디터/싱글: 현재 월드 컨텍스트에서 열기
    UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), true, Options);
}
