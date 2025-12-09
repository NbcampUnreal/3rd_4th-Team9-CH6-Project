//#pragma once
//
//#include "Abilities/GameplayAbility.h"
//#include "RSInteractionInfo.generated.h"
//
//class IRSInteractable;
//
//USTRUCT(BlueprintType)
//struct FRSInteractionInfo
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(BlueprintReadWrite)
//	TScriptInterface<IRSInteractable> Interactable;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FText Title;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FText Content;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	float Duration = 0.f;
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	TSubclassOf<UGameplayAbility> AbilityToGrant;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	TObjectPtr<UAnimMontage> ActiveStartMontage;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	TObjectPtr<UAnimMontage> ActiveEndMontage;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayCue"))
//	FGameplayTag ActiveLoopGameplayCueTag;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	TSoftClassPtr<UUserWidget> InteractionWidgetClass;
//
//public:
//	FORCEINLINE bool operator==(const FRSInteractionInfo& Other) const
//	{
//		return Interactable == Other.Interactable &&
//			Title.IdenticalTo(Other.Title) &&
//			Content.IdenticalTo(Other.Content) &&
//			Duration == Other.Duration &&
//			AbilityToGrant == Other.AbilityToGrant &&
//			ActiveStartMontage == Other.ActiveStartMontage &&
//			ActiveEndMontage == Other.ActiveEndMontage &&
//			ActiveLoopGameplayCueTag == Other.ActiveLoopGameplayCueTag &&
//			InteractionWidgetClass == Other.InteractionWidgetClass;
//	}
//
//	FORCEINLINE bool operator!=(const FRSInteractionInfo& Other) const
//	{
//		return !operator==(Other);
//	}
//
//	FORCEINLINE bool operator<(const FRSInteractionInfo& Other) const
//	{
//		return Interactable.GetInterface() < Other.Interactable.GetInterface();
//	}
//};
