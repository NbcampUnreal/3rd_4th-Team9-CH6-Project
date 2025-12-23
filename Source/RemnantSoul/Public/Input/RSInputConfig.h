#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RSInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FRSInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMapping = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;
};

USTRUCT(BlueprintType)
struct FRSInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "RS|Input")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "RS|Input")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FRSInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FRSInputAction> AbilityInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FRSInputMappingContextAndPriority> DefaultMappings;
};
