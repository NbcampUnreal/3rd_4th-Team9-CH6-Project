#include "Input/RSInputConfig.h"
#include "InputAction.h"

const UInputAction* URSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FRSInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
			return Action.InputAction.Get();
	}
	return nullptr;
}

const UInputAction* URSInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FRSInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
			return Action.InputAction.Get();
	}
	return nullptr;
}
