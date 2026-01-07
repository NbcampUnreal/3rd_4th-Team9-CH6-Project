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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	// 캐시 준비(필요할 때 1회 빌드)
	void EnsureCachesBuilt() const;

	// 캐시를 다시 만들기
	void RebuildCaches() const;

private:
	// Const DataAsset이라도, 런타임 캐시는 mutable로 유지 가능
	mutable bool bCacheBuilt = false;
	mutable TMap<FGameplayTag, TObjectPtr<const UInputAction>> NativeActionMap;
	mutable TMap<FGameplayTag, TObjectPtr<const UInputAction>> AbilityActionMap;
};
