#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CVUserFacingExperienceDefinition.generated.h"

UCLASS(BlueprintType)
class CHRONOVEIL_API UCVUserFacingExperienceDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:

    /** 에디터/싱글에서 바로 레벨 여는 런처 */ // HakClass와 차이점
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Experience")
    void OpenInEditor() const;

    /** 로드할 맵 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience", meta = (AllowedTypes = "Map"))
    FPrimaryAssetId MapID;

    /** 사용할 게임플레이 경험(아래 CVExperienceDefinition) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience", meta = (AllowedTypes = "CVExperienceDefinition"))
    FPrimaryAssetId ExperienceID;
};
