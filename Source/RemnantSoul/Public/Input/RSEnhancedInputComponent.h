#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/RSInputConfig.h"
#include "RemnantSoul.h" // LogRemnantSoul
#include "RSEnhancedInputComponent.generated.h"

USTRUCT()
struct FRSAbilityBindEvents
{
	GENERATED_BODY()

	ETriggerEvent PressedEvent = ETriggerEvent::Started;
	ETriggerEvent ReleasedEvent = ETriggerEvent::Completed;

	bool bBindCanceledAsReleased = true;
	bool bBindTriggeredAsPressed = false;
};

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
		PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
		TArray<uint32>& BindHandles,
		bool bLogIfInvalid = true,
		const FRSAbilityBindEvents& Events = FRSAbilityBindEvents())
	{
		check(InputConfig);

		// 같은 InputAction이 여러 태그에 들어간 실수 탐지(선택)
		TSet<const UInputAction*> SeenActions;

		for (const FRSInputAction& Action : InputConfig->AbilityInputActions)
		{
			// 1) Tag 유효성
			if (!Action.InputTag.IsValid())
			{
				if (bLogIfInvalid)
				{
					UE_LOG(LogRemnantSoul, Warning,
						TEXT("[Input] AbilityInputAction has invalid tag. Config=%s"),
						*GetNameSafe(InputConfig));
				}
				continue;
			}

			// 2) Action 유효성
			if (!Action.InputAction)
			{
				if (bLogIfInvalid)
				{
					UE_LOG(LogRemnantSoul, Warning,
						TEXT("[Input] AbilityInputAction Tag=%s has null action. Config=%s"),
						*Action.InputTag.ToString(), *GetNameSafe(InputConfig));
				}
				continue;
			}

			const UInputAction* IA = Action.InputAction.Get();
			const FGameplayTag Tag = Action.InputTag;

			// 3) 중복 IA 탐지(선택)
			if (SeenActions.Contains(IA))
			{
				if (bLogIfInvalid)
				{
					UE_LOG(LogRemnantSoul, Error,
						TEXT("[Input] Duplicate UInputAction in AbilityInputActions. IA=%s Config=%s"),
						*GetNameSafe(IA), *GetNameSafe(InputConfig));
				}
			}
			SeenActions.Add(IA);

			// 4) Pressed 바인딩
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(IA, Events.PressedEvent, Object, PressedFunc, Tag).GetHandle());

				if (Events.bBindTriggeredAsPressed)
				{
					BindHandles.Add(BindAction(IA, ETriggerEvent::Triggered, Object, PressedFunc, Tag).GetHandle());
				}
			}

			// 5) Released 바인딩
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(IA, Events.ReleasedEvent, Object, ReleasedFunc, Tag).GetHandle());

				if (Events.bBindCanceledAsReleased)
				{
					BindHandles.Add(BindAction(IA, ETriggerEvent::Canceled, Object, ReleasedFunc, Tag).GetHandle());
				}
			}
		}
	}

	void RemoveBindingsByHandleArray(TArray<uint32>& BindHandles)
	{
		for (uint32 Handle : BindHandles)
		{
			RemoveBindingByHandle(Handle);
		}
		BindHandles.Reset();
	}
};
