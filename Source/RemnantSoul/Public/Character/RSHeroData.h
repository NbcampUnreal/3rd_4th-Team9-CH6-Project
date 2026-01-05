
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSHeroData.generated.h"

class URSPawnData;
class URSCombatStyleData;
class URSItemTemplate;

UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSHeroData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 이 영웅이 사용하는 Base PawnData
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Hero")
	TObjectPtr<URSPawnData> PawnData = nullptr;

	// 무기가 없을 때의 기본 전투 스타일(맨손 콤보 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Hero")
	TObjectPtr<URSCombatStyleData> DefaultUnarmedStyle = nullptr;

	// (선택) 시작 장비/템플릿들 (원하면 여기서 스타팅 인벤토리 구성)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Hero")
	TArray<TObjectPtr<URSItemTemplate>> StartingItems;
};
