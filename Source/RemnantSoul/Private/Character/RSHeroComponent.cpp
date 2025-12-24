#include "Character/RSHeroComponent.h"
#include "Character/RSCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Input/RSEnhancedInputComponent.h"
#include "Input/RSInputConfig.h"
#include "AbilitySystemBlueprintLibrary.h" // 래퍼함수안 쓸 때 - 안쓰고서 그냥 HeroComponent와 InputConfig클래스를 이용할 예정임.
#include "RSGameplayTags.h"

void URSHeroComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC) return;

	InitializePlayerInput(PlayerInputComponent, PC);
}

void URSHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("[Hero] InputCompClass=%s"),
		*GetNameSafe(PlayerInputComponent ? PlayerInputComponent->GetClass() : nullptr));

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Cast URSEnhancedInputComponent=%s"),
		Cast<URSEnhancedInputComponent>(PlayerInputComponent) ? TEXT("OK") : TEXT("FAIL"));

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Cast UEnhancedInputComponent=%s"),
		Cast<UEnhancedInputComponent>(PlayerInputComponent) ? TEXT("OK") : TEXT("FAIL"));


	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	ULocalPlayer* LP = PlayerController->GetLocalPlayer();
	if (!LP) return;

	auto* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem) return;

	Subsystem->ClearAllMappings();

	const URSInputConfig* InputConfig = Char->GetInputConfig(); 
	if (!InputConfig) return;

	if (!Char->GetPawnData())
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero] PawnData is null on %s"), *GetNameSafe(Char));
		return;
	}

	for (const auto& Mapping : InputConfig->DefaultMappings)
	{
		if (UInputMappingContext* IMC = Mapping.InputMapping)
		{
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Owner=%s PC=%s InputComp=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(PlayerController),
		*GetNameSafe(PlayerInputComponent));

	URSEnhancedInputComponent* IC = Cast<URSEnhancedInputComponent>(PlayerInputComponent);
	if (!IC) return;

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Cast URSEnhancedInputComponent = %s"), IC ? TEXT("OK") : TEXT("FAIL"));

	// Ability 태그 바인딩
	TArray<uint32> Handles;
	IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityTagPressed, &ThisClass::Input_AbilityTagReleased, Handles);

	const FRSGameplayTags& RSGameplayTag = FRSGameplayTags::Get();
	// Native 태그 바인딩
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
}

void URSHeroComponent::Input_AbilityTagPressed(FGameplayTag InputTag)
{
	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	FGameplayEventData Payload;
	Payload.EventTag = InputTag;
	Payload.EventMagnitude = 1.f; // Pressed일때.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, InputTag, Payload);
}

void URSHeroComponent::Input_AbilityTagReleased(FGameplayTag InputTag)
{
	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	FGameplayEventData Payload;
	Payload.EventTag = InputTag;
	Payload.EventMagnitude = 0.f; // Released일떄.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, InputTag, Payload);
}

ARSCharacter* URSHeroComponent::GetOwnerCharacter() const
{
	return Cast<ARSCharacter>(GetOwner());
}

void URSHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	AController* Controller = Character ? Character->GetController() : nullptr;
	if (!IsValid(Controller)) return;

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if (!FMath::IsNearlyZero(Value.X))
	{
		const FVector Dir = YawRot.RotateVector(FVector::ForwardVector);
		Character->AddMovementInput(Dir, Value.X);
	}
	if (!FMath::IsNearlyZero(Value.Y))
	{
		const FVector Dir = YawRot.RotateVector(FVector::RightVector);
		Character->AddMovementInput(Dir, Value.Y);
	}
}

void URSHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!IsValid(Character)) return;

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (!FMath::IsNearlyZero(Value.X))
		Character->AddControllerYawInput(Value.X);

	if (!FMath::IsNearlyZero(Value.Y))
		Character->AddControllerPitchInput(Value.Y);
}
