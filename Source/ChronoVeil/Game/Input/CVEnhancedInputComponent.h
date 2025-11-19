//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameplayTagContainer.h"
//#include "EnhancedInputComponent.h"
//#include "InputAction.h"
//#include "CVInputConfig.h"
//#include "CVEnhancedInputComponent.generated.h"
//
//class UCVInputConfig;
//
///**
// * 경량 Lyra 스타일 입력 컴포넌트
// * - Native: 컨트롤러/캐릭터에서 직접 처리 (Move/Look 등)
// * - Ability: 태그 기반으로 ASC에 전달(Pressed/Released)
// */
//UCLASS()
//class CHRONOVEIL_API UCVEnhancedInputComponent : public UEnhancedInputComponent
//{
//	GENERATED_BODY()
//
//public:
//	UCVEnhancedInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/** 특정 태그의 Native 액션을 바인딩 */
//	template <class UserClass, typename FuncType>
//	void BindNativeAction(const UCVInputConfig* InputConfig,
//		const FGameplayTag& InputTag,
//		ETriggerEvent TriggerEvent,
//		UserClass* Object,
//		FuncType Func,
//		bool bLogIfNotFound = true);
//
//	/**
//	 * Ability 입력 전부 바인딩(Pressed/Released)
//	 * - OutBindHandles에 바인딩 핸들을 담아, UnbindAbilityActions로 해제 가능
//	 * - 핸들러 시그니처 예: void OnPressed(FGameplayTag Tag), void OnReleased(FGameplayTag Tag)
//	 */
//	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
//	void BindAbilityActions(const UCVInputConfig* InputConfig,
//		UserClass* Object,
//		PressedFuncType PressedFunc,
//		ReleasedFuncType ReleasedFunc,
//		TArray<uint32>& OutBindHandles);
//
//	/** BindAbilityActions로 묶은 바인딩 일괄 해제 */
//	void UnbindAbilityActions(TArray<uint32>& InOutBindHandles);
//
//	/** 단일 태그에 대해 Ability 바인딩(선택 기능) */
//	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
//	void BindAbilityActionByTag(const UCVInputConfig* InputConfig,
//		const FGameplayTag& InputTag,
//		UserClass* Object,
//		PressedFuncType PressedFunc,
//		ReleasedFuncType ReleasedFunc,
//		TArray<uint32>& OutBindHandles);
//};
//
//// ------------------ Template Implementations ------------------
//
//template <class UserClass, typename FuncType>
//FORCEINLINE void UCVEnhancedInputComponent::BindNativeAction(
//	const UCVInputConfig* InputConfig,
//	const FGameplayTag& InputTag,
//	ETriggerEvent TriggerEvent,
//	UserClass* Object,
//	FuncType Func,
//	bool bLogIfNotFound)
//{
//	check(InputConfig);
//
//	// Native는 대개 소비
//	bool bConsume = true;
//	if (const UInputAction* IA = InputConfig->FindNativeActionByTag(InputTag, bConsume))
//	{
//		// UE5.7 템플릿 오버로드: (const UInputAction*, ETriggerEvent, UserClass*, TMethodPtr, VarTypes...)
//		// Native 핸들러 예: void Input_Move(const FInputActionValue& Value)
//		BindAction(IA, TriggerEvent, Object, Func);
//	}
//	else if (bLogIfNotFound)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("BindNativeAction: No Native action for tag %s in %s"),
//			*InputTag.ToString(), *GetNameSafe(InputConfig));
//	}
//}
//
//template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
//FORCEINLINE void UCVEnhancedInputComponent::BindAbilityActions(
//	const UCVInputConfig* InputConfig,
//	UserClass* Object,
//	PressedFuncType PressedFunc,
//	ReleasedFuncType ReleasedFunc,
//	TArray<uint32>& OutBindHandles)
//{
//	check(InputConfig);
//
//	OutBindHandles.Reserve(OutBindHandles.Num() + InputConfig->AbilityInputActions.Num() * 2);
//
//	for (const auto& Action : InputConfig->AbilityInputActions)
//	{
//		// Action 구조가 TObjectPtr<UInputAction>인 경우를 가정
//		const UInputAction* IA = Action.InputAction.Get();
//		if (!IA) { continue; }
//
//		// 규칙: Pressed/Held → Triggered, Released → Completed
//		if (PressedFunc)
//		{
//			FEnhancedInputActionEventBinding& Binding =
//				BindAction(IA, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag);
//			OutBindHandles.Add(Binding.GetHandle());
//		}
//		if (ReleasedFunc)
//		{
//			FEnhancedInputActionEventBinding& Binding =
//				BindAction(IA, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
//			OutBindHandles.Add(Binding.GetHandle());
//		}
//	}
//}
//
//template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
//FORCEINLINE void UCVEnhancedInputComponent::BindAbilityActionByTag(
//	const UCVInputConfig* InputConfig,
//	const FGameplayTag& InputTag,
//	UserClass* Object,
//	PressedFuncType PressedFunc,
//	ReleasedFuncType ReleasedFunc,
//	TArray<uint32>& OutBindHandles)
//{
//	check(InputConfig);
//
//	// Ability 입력은 상위에서 소비하지 않음
//	const bool bConsume = false;
//	if (const UInputAction* IA = InputConfig->FindAbilityActionByTag(InputTag, bConsume))
//	{
//		if (PressedFunc)
//		{
//			FEnhancedInputActionEventBinding& Binding =
//				BindAction(IA, ETriggerEvent::Triggered, Object, PressedFunc, InputTag);
//			OutBindHandles.Add(Binding.GetHandle());
//		}
//		if (ReleasedFunc)
//		{
//			FEnhancedInputActionEventBinding& Binding =
//				BindAction(IA, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag);
//			OutBindHandles.Add(Binding.GetHandle());
//		}
//	}
//}










#pragma once

#include "EnhancedInputComponent.h"
#include "Game/Input/CVInputConfig.h"
#include "CVEnhancedInputComponent.generated.h"

class UInputAction;
class UCVInputConfig;
struct FGameplayTag;

UCLASS()
class CHRONOVEIL_API UCVEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UCVInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UCVInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

};

template<class UserClass, typename FuncType>
inline void UCVEnhancedInputComponent::BindNativeAction(const UCVInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
inline void UCVEnhancedInputComponent::BindAbilityActions(const UCVInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FCVInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}





