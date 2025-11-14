// 1.
//#include "Game/Character/CVHeroComponent.h"
//#include "Game/Character/CVCharacter.h"
//#include "Game/Character/CVPawnData.h"
//#include "Game/Character/CVPawnExtensionComponent.h"
//
//// 입력/카메라 관련
//
//#include "GameFramework/PlayerController.h"
//#include "GameFramework/Pawn.h"
//
//// TODO: 네 프로젝트 타입으로 교체
//#include "Game/Input/CVInputConfig.h"
//#include "Game/Input/CVEnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
//#include "InputMappingContext.h"
//
//// #include "Game/Camera/CVCameraComponent.h"
//// #include "Game/Camera/CVCameraMode.h"
//#include "Game/CVGameplayTags.h"
//
//#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
//
//UCVHeroComponent::UCVHeroComponent(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryComponentTick.bCanEverTick = false;
//}
//
//void UCVHeroComponent::HandlePossessed(ACVCharacter* Character)
//{
//	// 서버/클라 공통 초기화 시점
//	// ASC나 PawnExtension 초기화 등 필요한 최소한의 준비 가능
//}
//
//void UCVHeroComponent::HandleControllerChanged()
//{
//	APawn* Pawn = GetPawn<APawn>();
//	if (!Pawn) return;
//
//	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayerSubsystem())
//	{
//		// PMIC → IMC를 모두 펼쳐서 일괄 등록
//		TArray<UInputMappingContext*> IMCs;
//		TArray<int32> Priorities;
//
//		for (const FCVMappableConfigPair& Pair : DefaultInputConfigs)
//		{
//			Pair.ResolveMappingContexts(IMCs, Priorities);
//		}
//
//		for (int32 i = 0; i < IMCs.Num(); ++i)
//		{
//			if (IMCs[i])
//			{
//				const int32 Pri = Priorities.IsValidIndex(i) ? Priorities[i] : 0;
//				Subsystem->AddMappingContext(IMCs[i], Pri);
//			}
//		}
//	}
//
//	// 입력 컴포넌트 초기화
//	if (Pawn->InputComponent)
//	{
//		InitializePlayerInput(Pawn->InputComponent);
//	}
//}
//
////void UCVHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
////{
////	UCVEnhancedInputComponent* CVIC = Cast<UCVEnhancedInputComponent>(PlayerInputComponent);
////	if (!CVIC) return;
////
////	const UCVInputConfig* InputConfig = GetInputConfigSafe();
////	if (!InputConfig) return;
////
////	const FCVGameplayTags& Tags = FCVGameplayTags::Get();
////
////	// ────────────────────────────────
////	// Ability Input Binding
////	// ────────────────────────────────
////	{
////		TArray<uint32> Handles;
////		CVIC->BindAbilityActions(
////			InputConfig,
////			this,
////			&ThisClass::Input_AbilityInputTagPressed,
////			&ThisClass::Input_AbilityInputTagReleased,
////			Handles
////		);
////	}
////
////	// ────────────────────────────────
////	// Native Movement / Look Binding
////	// ────────────────────────────────
////	CVIC->BindNativeAction(InputConfig, Tags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
////	CVIC->BindNativeAction(InputConfig, Tags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
////}
//
//void UCVHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
//{
//	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
//	if (!EIC) return;
//
//	const UCVInputConfig* InputConfig = GetInputConfigSafe();
//	if (!InputConfig) return;
//
//	const FCVGameplayTags& Tags = FCVGameplayTags::Get();
//
//	// 1) Ability 입력(태그 기반) 바인딩
//	{
//		TArray<uint32> Handles;
//		// 만약 네가 CVEnhancedInputComponent에만 있는 헬퍼를 쓰고 있었다면,
//		// 여기서는 InputConfig 내부 목록을 직접 순회해서 BindAction 해주면 됨.
//		for (const auto& Def : InputConfig->AbilityInputActions)
//		{
//			if (Def.InputAction && Def.InputTag.IsValid())
//			{
//				// Pressed
//				EIC->BindAction(Def.InputAction, ETriggerEvent::Started, this,
//					&ThisClass::Input_AbilityInputTagPressed, Def.InputTag);
//				// Released
//				EIC->BindAction(Def.InputAction, ETriggerEvent::Completed, this,
//					&ThisClass::Input_AbilityInputTagReleased, Def.InputTag);
//			}
//		}
//	}
//
//	// 2) 네이티브 이동/룩 바인딩
//	{
//		bool bConsume = false; // 또는 변수 이름을 네 API 의미에 맞게
//		if (const UInputAction* IA_Move = InputConfig->FindNativeActionByTag(Tags.InputTag_Move, bConsume))
//		{
//			EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
//		}
//	}
//
//	{
//		bool bConsume = false;
//		if (const UInputAction* IA_Look = InputConfig->FindNativeActionByTag(Tags.InputTag_Look_Mouse, bConsume))
//		{
//			EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
//		}
//	}
//}
//
//
//void UCVHeroComponent::Input_Move(const FInputActionValue& ActionValue)
//{
//	APawn* Pawn = GetPawn<APawn>();
//	if (!Pawn) return;
//
//	AController* Controller = Pawn->GetController();
//	if (!Controller) return;
//
//	const FVector2D Value = ActionValue.Get<FVector2D>();
//	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
//
//	if (Value.X != 0.f)
//		Pawn->AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), Value.X);
//	if (Value.Y != 0.f)
//		Pawn->AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), Value.Y);
//}
//
//void UCVHeroComponent::Input_LookMouse(const FInputActionValue& ActionValue)
//{
//	if (APawn* Pawn = GetPawn<APawn>())
//	{
//		const FVector2D Value = ActionValue.Get<FVector2D>();
//		Pawn->AddControllerYawInput(Value.X);
//		Pawn->AddControllerPitchInput(-Value.Y);
//	}
//}
//
//void UCVHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
//{
//	if (const APawn* Pawn = GetPawn<APawn>())
//	{
//		if (const UCVPawnExtensionComponent* Ext = Pawn->FindComponentByClass<UCVPawnExtensionComponent>())
//		{
//			if (UCVAbilitySystemComponent* ASC = Ext->GetCVAbilitySystemComponent())
//			{
//				ASC->AbilityInputTagPressed(InputTag);
//			}
//		}
//	}
//}
//
//void UCVHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
//{
//	if (const APawn* Pawn = GetPawn<APawn>())
//	{
//		if (const UCVPawnExtensionComponent* Ext = Pawn->FindComponentByClass<UCVPawnExtensionComponent>())
//		{
//			if (UCVAbilitySystemComponent* ASC = Ext->GetCVAbilitySystemComponent())
//			{
//				ASC->AbilityInputTagReleased(InputTag);
//			}
//		}
//	}
//}
//
//UEnhancedInputLocalPlayerSubsystem* UCVHeroComponent::GetLocalPlayerSubsystem() const
//{
//	const APawn* Pawn = GetPawn<APawn>();
//	const APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
//	const ULocalPlayer* LP = PC ? PC->GetLocalPlayer() : nullptr;
//	return LP ? LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
//}
//
//const UCVPawnData* UCVHeroComponent::GetPawnDataSafe() const
//{
//	const APawn* Pawn = GetPawn<APawn>();
//	if (const UCVPawnExtensionComponent* Ext = Pawn ? Pawn->FindComponentByClass<UCVPawnExtensionComponent>() : nullptr)
//	{
//		return Ext->GetPawnData<UCVPawnData>();
//	}
//	return nullptr;
//}
//
//const UCVInputConfig* UCVHeroComponent::GetInputConfigSafe() const
//{
//	if (const UCVPawnData* PD = GetPawnDataSafe())
//	{
//		return PD->InputConfig;
//	}
//	return nullptr;
//}
//
//TSubclassOf<UCVCameraMode> UCVHeroComponent::DetermineCameraMode() const
//{
//	if (const UCVPawnData* PD = GetPawnDataSafe())
//	{
//		return PD->DefaultCameraMode;
//	}
//	return nullptr;
//}



#include "Game/Character/CVHeroComponent.h"
#include "Game/Character/CVCharacter.h"
#include "Game/Character/CVPawnData.h"
#include "EnhancedInputSubsystems.h"
//#include "Game/Input/CVUserFacingExperienceDefinition.h"
#include "Game/Input/CVEnhancedInputComponent.h"
#include "Game/Input/CVInputConfig.h"
#include "Game/Camera/CVCameraComponent.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
#include "Game/CVGameplayTags.h"


UCVHeroComponent::UCVHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCVHeroComponent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	APawn* ComponentOwner = GetOwner<APawn>();
	if (IsValid(ComponentOwner))
	{
		APlayerController* PlayerController = ComponentOwner->GetController<APlayerController>();
		if (IsValid(PlayerController))
		{
			InitializePlayerInput(PlayerInputComponent, PlayerController);
		}
	}
}

TSubclassOf<UCVCameraMode> UCVHeroComponent::DetermineCameraMode() const
{
	const ACVCharacter* CVChar = GetOwnerCharacter();
	if (!CVChar)
	{
		return nullptr;
	}

	// Aim중이면 ADS모드로 결정.
	if (bIsAiming && AimCameraModeClass)
	{
		return AimCameraModeClass;
	}

	if (const UCVPawnData* PawnData = CVChar->GetPawnData())
	{
		return PawnData->DefaultCameraMode;
	}
	return nullptr;
}

void UCVHeroComponent::TryBindCameraMode()
{
	APawn* Pawn = GetOwner<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (!Pawn->IsLocallyControlled())
	{
		// 로컬만 카메라 제어
		return;
	}

	// PawnData가 아직 복제 전이면 여기서 실패할 수 있음
	const ACVCharacter* CVChar = GetOwnerCharacter();
	if (!CVChar || !CVChar->GetPawnData())
	{
		return;
	}

	if (UCVCameraComponent* Camera = UCVCameraComponent::FindCameraComponent(Pawn))
	{
		if (!Camera->DetermineCameraModeDelegate.IsBound())
		{
			Camera->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}
}

void UCVHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	TryBindCameraMode();
}

void UCVHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UCVHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController)
{
	ACVCharacter* CVCharacter = GetOwnerCharacter();
	check(CVCharacter);

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UCVPawnData* PawnData = CVCharacter->GetPawnData())
	{
		if (const UCVInputConfig* InputConfig = PawnData->InputConfig)
		{

			for (const FInputMappingContextAndPriority Mapping : InputConfig->DefaultMappings)
			{
				if (UInputMappingContext* IMC = Mapping.InputMapping)
				{
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
				}
			}

			UCVEnhancedInputComponent* InputComponent = Cast<UCVEnhancedInputComponent>(PlayerInputComponent);
			check(InputComponent);

			TArray<uint32> OutBindHandles;
			InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, OutBindHandles);


			const FCVGameplayTags& CVGameplayTag = FCVGameplayTags::Get();
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Look_Aim, ETriggerEvent::Started, this, &ThisClass::OnAimStart);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Look_Aim, ETriggerEvent::Completed, this, &ThisClass::OnAimEnd);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch);

		}
	}
}

void UCVHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	ACVCharacter* Character = GetOwnerCharacter();
	if (IsValid(Character))
	{
		UCVAbilitySystemComponent* CVASC = Character->GetCVAbilitySystemComponent();
		if (IsValid(CVASC))
		{
			CVASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UCVHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	ACVCharacter* Character = GetOwnerCharacter();
	if (IsValid(Character))
	{
		UCVAbilitySystemComponent* CVASC = Character->GetCVAbilitySystemComponent();
		if (IsValid(CVASC))
		{
			CVASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UCVHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();
	AController* Controller = Character ? Character->GetController() : nullptr;

	if (IsValid(Controller))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (!FMath::IsNearlyZero(Value.X))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Character->AddMovementInput(MovementDirection, Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Character->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UCVHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();

	if (IsValid(Character))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if (!FMath::IsNearlyZero(Value.X))
		{
			Character->AddControllerYawInput(Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			Character->AddControllerPitchInput(Value.Y);
		}
	}
}

void UCVHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();

	if (IsValid(Character))
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}
	}
}

void UCVHeroComponent::Input_Aim(const FInputActionValue& InputActionValue)
{
	const bool bIsPressed = InputActionValue.Get<bool>();
	bIsAiming = bIsPressed;
	UE_LOG(LogTemp, Log, TEXT("Aim pressed? %s"), bIsPressed ? TEXT("true") : TEXT("false"));

}

void UCVHeroComponent::OnAimStart(const FInputActionValue& Value)
{
	bIsAiming = true;
}

void UCVHeroComponent::OnAimEnd(const FInputActionValue& Value)
{
	bIsAiming = false;
}

ACVCharacter* UCVHeroComponent::GetOwnerCharacter()
{
	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		return Cast<ACVCharacter>(ComponentOwner);
	}
	return nullptr;
}

const ACVCharacter* UCVHeroComponent::GetOwnerCharacter() const
{
	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		return Cast<const ACVCharacter>(ComponentOwner);
	}
	return nullptr;
}


