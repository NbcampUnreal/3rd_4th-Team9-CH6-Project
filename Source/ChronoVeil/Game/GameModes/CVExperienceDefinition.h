// CVExperienceDefinition.h
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h" //  UE5.7에선 PrimaryDataAsset.h이 DataAsset.h로 통합됨.
#include "CVExperienceDefinition.generated.h"

class UCVPawnData; // 너의 기존 Hak PawnData를 그대로 재사용


/**
 * 경험(Experience) 기본 설정 묶음
 * - DefaultPawnDataClass: 이 경험에서 사용할 기본 PawnData 클래스 (CDO를 즉시 얻어쓰기 위함)
 */
UCLASS(BlueprintType)
class CHRONOVEIL_API UCVExperienceDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // 생성자 (CPP에 구현되어 있다면 선언 필요)
    UCVExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // PrimaryAssetType을 명시적으로 CVExperienceDefinition으로 고정하고 싶으면 override
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;

    // DefaultPawnDataClass의 CDO를 돌려줌 (CPP에 구현되어 있으니 선언 필요)
    UFUNCTION(BlueprintCallable, Category = "Experience")
    const UCVPawnData* GetDefaultPawnData() const;

public:
    // CPP에서 사용하는 이름과 동일하게 맞춤
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
    TSoftObjectPtr<UCVPawnData> DefaultPawnData; // ← CPP에서 쓰는 이름으로 변경
    //TObjectPtr<UCVPawnData> DefaultPawnDataClass;

};