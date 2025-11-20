//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Engine/DataAsset.h"
//#include "GameplayTagContainer.h"
//#include "CVMappableConfigPair.h"   // 경로는 실제 폴더에 맞게
//#include "CVInputConfig.generated.h"
//
//class UInputAction;
//class UInputMappingContext;
//
//
///** 입력 레이어 (IMC 우선순위/전환 관리용) // 추후에 GameplayTag로 관리해야할까 고민중임. */
//UENUM(BlueprintType)
//enum class ECVInputLayer : uint8
//{
//	KBM     UMETA(DisplayName = "Keyboard&Mouse"),
//	Pad     UMETA(DisplayName = "Gamepad"),
//	Drone   UMETA(DisplayName = "Drone"),
//	Vehicle UMETA(DisplayName = "Vehicle"),
//	Debug   UMETA(DisplayName = "Debug")
//};
//
///**
// * 한 액션과 GameplayTag를 매핑하는 엔트리
// * - Native용/Ability용 모두 동일 구조 사용
// */
//USTRUCT(BlueprintType)
//struct FCVTaggedInputAction
//{
//	GENERATED_BODY()
//
//public:
//	// Enhanced Input의 액션
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
//	TObjectPtr<UInputAction> InputAction = nullptr;
//
//	// 이 액션을 식별/바인딩하는 태그 (예: InputTag.Move, InputTag.Look.Mouse, InputTag.Ability.Drone)
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (Categories = "InputTag"))
//	FGameplayTag InputTag;
//
//	// 컨트롤러 단계에서 소비할지 여부(보통 Native는 true, Ability는 false 권장)
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
//	bool bConsumeInput = true;
//
//	bool IsValid() const { return InputAction != nullptr && InputTag.IsValid(); }
//};
//
///**
// * 경량 Lyra 스타일 입력 구성
// * - NativeInputActions: 컨트롤러/캐릭터에서 직접 처리(이동/시점 등)
// * - AbilityInputActions: GameplayTag 큐로 ASC에 전달하여 Ability 활성화
// * - MappableConfigs: IMC 묶음(PMI_Default_KBM 등)과 우선순위
// */
//UCLASS()
//class CHRONOVEIL_API UCVInputConfig : public UDataAsset
//{
//	GENERATED_BODY()
//
//public:
//	/** 이동/시점 등 네이티브 처리 액션 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Native")
//	TArray<FCVTaggedInputAction> NativeInputActions;
//
//	/** Ability 입력(누름/홀드/뗌)을 태그로 ASC에 전달 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Ability")
//	TArray<FCVTaggedInputAction> AbilityInputActions;
//
//	/** 기본/드론 등 매핑 컨텍스트 세트 */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Mappings")
//	TArray<FCVMappableConfigPair> MappableConfigs;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
//	TArray<FInputMappingContextAndPriority> DefaultMappings;
//
//
//public:
//	/** 태그로 Native 액션 조회 (없으면 nullptr) */
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	UInputAction* FindNativeActionByTag(const FGameplayTag& Tag, bool& bOutConsume) const;
//
//	/** 태그로 Ability 액션 조회 (없으면 nullptr) */
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	UInputAction* FindAbilityActionByTag(const FGameplayTag& Tag, bool& bOutConsume) const;
//
//	/** 디폴트 활성화 대상 IMC들 반환 (non-const 포인터) */
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	void GetDefaultMappableConfigs(TArray<UInputMappingContext*>& OutIMCs,
//		TArray<int32>& OutPriorities) const;
//
//	/** 특정 레이어의 IMC들만 추출 (예: Drone/Vehicle 전환용) */
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	void GetMappableConfigsByLayerTag(const FGameplayTag& LayerTag,
//		TArray<UInputMappingContext*>& OutIMCs,
//		TArray<int32>& OutPriorities) const;
//
//	/** 태그 존재 여부 빠른 확인 */
//	UFUNCTION(BlueprintCallable, Category = "Input")
//	bool HasInputTag(const FGameplayTag& Tag) const;
//};




#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CVInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;
};


// Pair of (Gameplay Tag, Input Action)
USTRUCT(BlueprintType)
struct FCVInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


UCLASS(BlueprintType, Const)
class CHRONOVEIL_API UCVInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UCVInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FCVInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FCVInputAction> AbilityInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FInputMappingContextAndPriority> DefaultMappings;
};
