//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Engine/DataAsset.h"
//#include "RSCombatStyleData.generated.h"
//
///**
// * 
// */
//UCLASS()
//class REMNANTSOUL_API URSCombatStyleData : public UDataAsset
//{
//	GENERATED_BODY()
//	
//};



#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSCombatStyleData.generated.h"

class URSAbilitySet;
class URSInputConfig;

UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSCombatStyleData : public UDataAsset
{
	GENERATED_BODY()
public:
	// 이 스타일을 식별하는 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Style")
	FGameplayTag StyleTag;

	// 이 스타일이 부여하는 AbilitySets (검 전용, 스태프 전용 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Style")
	TArray<TObjectPtr<URSAbilitySet>> AbilitySets;

	// 이 스타일이 얹는 Overlay 입력(공격/스킬 등). 이동/시점 같은 Base는 PawnData에 둔다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Style")
	TObjectPtr<URSInputConfig> OverlayInputConfig = nullptr;

	// 상체/전투 애님 레이어 전환용 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Style")
	FGameplayTag AnimLayerTag;
};
