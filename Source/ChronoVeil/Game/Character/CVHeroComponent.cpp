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
	if (!ensureAlways(CVCharacter))
	{
		return;
	}
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!ensureAlways(LocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!ensureAlways(Subsystem))
	{
		return;
	}

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
			if (!ensureAlwaysMsgf(InputComponent, TEXT("HeroComponent: Expected UCVEnhancedInputComponent but got %s"),
				*GetNameSafe(PlayerInputComponent ? PlayerInputComponent->GetClass() : nullptr)))
			{
				return;
			}

			TArray<uint32> OutBindHandles;
			InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, OutBindHandles);


			const FCVGameplayTags& CVGameplayTag = FCVGameplayTags::Get();
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Native_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Native_Look_Aim, ETriggerEvent::Started, this, &ThisClass::OnAimStart);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Native_Look_Aim, ETriggerEvent::Completed, this, &ThisClass::OnAimEnd);
			InputComponent->BindNativeAction(InputConfig, CVGameplayTag.InputTag_Native_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch);

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


