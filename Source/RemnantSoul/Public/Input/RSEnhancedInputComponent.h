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
            {
                continue;
            }

            const UInputAction* IA = Action.InputAction.Get();
            const FGameplayTag Tag = Action.InputTag;

            if (PressedFunc)
            {
                BindHandles.Add(
                    BindAction(IA, ETriggerEvent::Started, Object, PressedFunc, Tag).GetHandle()
                );
            }

            if (ReleasedFunc)
            {
                // Released는 Completed만으로 부족할 수 있음
                BindHandles.Add(
                    BindAction(IA, ETriggerEvent::Completed, Object, ReleasedFunc, Tag).GetHandle()
                );

                // 키 뗄 때 Canceled로 떨어지는 케이스 대응
                BindHandles.Add(
                    BindAction(IA, ETriggerEvent::Canceled, Object, ReleasedFunc, Tag).GetHandle()
                );
            }
        }
    }

};
