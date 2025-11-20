#pragma once

#include "Engine/DataAsset.h"
//#include "Cosmetics/CVCharacterPartType.h" // Cosmetic 난이도가 높아서 나중에 적용할지 말지 결정할 예정.
#include "CVPawnData.generated.h"

class UCVAbilitySet;
class UCVInputConfig;
class UCVCameraMode;

/**
 * Pawn을 정의하기 위한 속성(properties)를 가지고 있는 Data Asset
 * ULyraPawnData의 구성을 참고함
 */
UCLASS(BlueprintType, Const)
class CHRONOVEIL_API UCVPawnData : public UDataAsset
{
	GENERATED_BODY()

public:
	UCVPawnData(const FObjectInitializer& ObjectInitializer);

	// Pawn 또는 Character에 부여할(grant) Ability 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnData|Abilities")
	TArray<TObjectPtr<UCVAbilitySet>> AbilitySets;

	// Pawn 또는 Character의 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnData|Input")
	TObjectPtr<UCVInputConfig> InputConfig;

	//// Pawn 또는 Character에 적용할 Cosmetic(외형) 정보
	//UPROPERTY(EditAnywhere, Category = "PawnData|Cosmetics")
	//TArray<FCVCharacterPart> InitCharacterParts;

	//UPROPERTY(EditAnywhere, Category = "PawnData|Cosmetics")
	//FCVAnimBodyStyleSelectionSet InitBodyMeshes;

	// Pawn 또는 Character에 적용할 기본 카메라 모드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PawnData|Camera")
	TSubclassOf<UCVCameraMode> DefaultCameraMode;
};
