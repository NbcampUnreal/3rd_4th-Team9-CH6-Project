#pragma once

#include "EnhancedInputComponent.h"
#include "RSInputConfig.h"
#include "RSEnhancedInputComponent.generated.h"

UCLASS()
class REMNANTSOUL_API URSEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(const URSInputConfig* InputConfig, const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true)
	{
		check(InputConfig);
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
		{
			BindAction(IA, TriggerEvent, Object, Func);
		}
	}

    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityActions(const URSInputConfig* InputConfig, UserClass* Object,
        PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
    {
        check(InputConfig);

        for (const FRSInputAction& Action : InputConfig->AbilityInputActions)
        {
            if (!Action.InputAction || !Action.InputTag.IsValid())
                continue;

            if (PressedFunc)
            {
                BindHandles.Add(
                    BindAction(Action.InputAction.Get(), ETriggerEvent::Started, Object, PressedFunc, Action.InputTag).GetHandle()
                );
            }

            if (ReleasedFunc)
            {
                BindHandles.Add(
                    BindAction(Action.InputAction.Get(), ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle()
                );
            }
        }
    }
};
