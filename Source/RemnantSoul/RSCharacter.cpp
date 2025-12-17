#include "RSCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"

ARSCharacter::ARSCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

void ARSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("[RS] LookAction=%s"), LookAction ? *LookAction->GetName() : TEXT("NULL"));

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);

	//EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	//EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::HandleGameplayAbilityInputPressed, 1);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::HandleGameplayAbilityInputReleased, 1);
		
		EnhancedInputComponent->BindAction(SuperJumpAction, ETriggerEvent::Triggered, this, &ThisClass::HandleGameplayAbilityInputPressed, 3);
		EnhancedInputComponent->BindAction(SuperJumpAction, ETriggerEvent::Completed, this, &ThisClass::HandleGameplayAbilityInputReleased, 3);


		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::HandleGameplayAbilityInputPressed, 2);
	}
}

void ARSCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

	UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

	EILPS->AddMappingContext(InputMappingContext, 0);

	ASC->InitAbilityActorInfo(this, this);

	for (const auto& GrantedAbility : GrantedAbilities)
	{
		FGameplayAbilitySpec GrantedAbilitySpec(GrantedAbility);
		ASC->GiveAbility(GrantedAbilitySpec);
	}

	for (const auto& GrantedInputAbility : GrantedInputAbilities)
	{
		FGameplayAbilitySpec GrantedAbilitySpec(GrantedInputAbility.Value);
		GrantedAbilitySpec.InputID = GrantedInputAbility.Key;
		ASC->GiveAbility(GrantedAbilitySpec);
	}

	if (IsValid(GetController()) == true)
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		PlayerController->ConsoleCommand(TEXT("ShowDebug AbilitySystem"));
	}
}

void ARSCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void ARSCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void ARSCharacter::HandleGameplayAbilityInputPressed(int32 InInputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InInputID);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void ARSCharacter::HandleGameplayAbilityInputReleased(int32 InInputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InInputID);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

UAbilitySystemComponent* ARSCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}