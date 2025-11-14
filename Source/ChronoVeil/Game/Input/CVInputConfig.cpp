//#include "Game/Input/CVInputConfig.h"
//#include "InputAction.h"
//#include "InputMappingContext.h"
//
//static UInputAction* FindActionInternal(const TArray<FCVTaggedInputAction>& List,
//    const FGameplayTag& Tag, bool& bOutConsume)
//{
//    for (const auto& It : List)
//    {
//        if (It.InputTag == Tag && It.IsValid())
//        {
//            bOutConsume = It.bConsumeInput;
//            return It.InputAction.Get();
//        }
//    }
//    bOutConsume = true;
//    return nullptr;
//}
//
//UInputAction* UCVInputConfig::FindNativeActionByTag(const FGameplayTag& Tag, bool& bOutConsume) const
//{
//    return FindActionInternal(NativeInputActions, Tag, bOutConsume);
//}
//
//UInputAction* UCVInputConfig::FindAbilityActionByTag(const FGameplayTag& Tag, bool& bOutConsume) const
//{
//    return FindActionInternal(AbilityInputActions, Tag, bOutConsume);
//}
//
//void UCVInputConfig::GetDefaultMappableConfigs(TArray<UInputMappingContext*>& OutIMCs,
//    TArray<int32>& OutPriorities) const
//{
//    OutIMCs.Reset();
//    OutPriorities.Reset();
//
//    for (const FCVMappableConfigPair& Pair : MappableConfigs)
//    {
//        if (Pair.bActivateByDefault && Pair.IsValid())
//        {
//            Pair.ResolveMappingContexts(OutIMCs, OutPriorities);
//        }
//    }
//}
//
//void UCVInputConfig::GetMappableConfigsByLayerTag(const FGameplayTag& LayerTag,
//    TArray<UInputMappingContext*>& OutIMCs,
//    TArray<int32>& OutPriorities) const
//{
//    OutIMCs.Reset();
//    OutPriorities.Reset();
//
//    if (!LayerTag.IsValid())
//    {
//        return;
//    }
//
//    for (const FCVMappableConfigPair& Pair : MappableConfigs)
//    {
//        if (Pair.IsValid() && Pair.LayerTag.IsValid() && Pair.LayerTag.MatchesTag(LayerTag))
//        {
//            Pair.ResolveMappingContexts(OutIMCs, OutPriorities);
//        }
//    }
//}
//
//bool UCVInputConfig::HasInputTag(const FGameplayTag& Tag) const
//{
//    for (const auto& It : NativeInputActions)  if (It.InputTag == Tag) return true;
//    for (const auto& It : AbilityInputActions) if (It.InputTag == Tag) return true;
//    return false;
//}






#include "Game/Input/CVInputConfig.h"

UCVInputConfig::UCVInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UCVInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FCVInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UCVInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FCVInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}


