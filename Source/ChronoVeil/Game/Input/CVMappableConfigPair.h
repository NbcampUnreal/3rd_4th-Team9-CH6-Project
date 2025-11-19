//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameplayTagContainer.h"                // ECVInputLayer 재사용
//#include "PlayerMappableInputConfig.h"
//#include "CVMappableConfigPair.generated.h"
//
//class UInputMappingContext;
//class UPlayerMappableInputConfig;
//
//
///** IMC 또는 PMIC 내부 IMC 세트를 활성화하기 위한 페어(레이어는 GameplayTag) */
//USTRUCT(BlueprintType)
//struct FCVMappableConfigPair
//{
//    GENERATED_BODY()
//
//    /** 단일 매핑 컨텍스트 (직접 AddMappingContext) */
//    UPROPERTY(EditDefaultsOnly, Category = "CV|Input")
//    TSoftObjectPtr<UInputMappingContext> InputMapping;
//
//    /** PlayerMappableInputConfig (선택) */
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
//    TSoftObjectPtr<UPlayerMappableInputConfig> PlayerMappableConfig;
//
//    /** AddMappingContext 우선순위 (값이 클수록 우선) */
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
//    int32 Priority = 0;
//
//    /** 로컬 접속 시 자동 활성화 여부 */
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
//    bool bActivateByDefault = true;
//
//    /** 입력 레이어 태그 (예: Input.Layer.Drone) */
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (Categories = "Input.Layer"))
//    FGameplayTag LayerTag;
//
//    bool IsValid() const { return (InputMapping != nullptr) || PlayerMappableConfig.IsValid(); }
//
//    /** (IMC, Priority) 목록으로 해석 */
//    void ResolveMappingContexts(TArray<UInputMappingContext*>& OutIMCs,
//        TArray<int32>& OutPriorities) const;
//};