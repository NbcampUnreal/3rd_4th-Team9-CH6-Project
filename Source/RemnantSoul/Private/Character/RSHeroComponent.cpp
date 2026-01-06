#include "Character/RSHeroComponent.h"
#include "Character/RSCharacter.h"
#include "PlayerState/RSPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "Input/RSEnhancedInputComponent.h"
#include "Input/RSInputConfig.h"
#include "AbilitySystemBlueprintLibrary.h" // 래퍼함수안 쓸 때 - 안쓰고서 그냥 HeroComponent와 InputConfig클래스를 이용할 예정임.
#include "AbilitySystemComponent.h"
#include "Character/RSHeroData.h"
#include "Character/RSPawnData.h"
#include "RSGameplayTags.h"

#include "GameFramework/PlayerController.h" // PRMerge할때 Conflict생긴 부분 주석처리해도 잘 작동하면 삭제해도될듯.
#include "GameFramework/Pawn.h"
#include "Character/PlayerController/RSPlayerController.h"

#include "Abilities/GameplayAbility.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Character/RSCombatStyleData.h"

#include "Input/RSInputConfig.h"


void URSHeroComponent::BeginPlay()
{
	Super::BeginPlay();


	if (ARSCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		EquipManager = OwnerCharacter->FindComponentByClass<URSEquipManagerComponent>();
	}
}

void URSHeroComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC ENTER Owner=%s InputComp=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(PlayerInputComponent));

	// 0) 같은 InputComponent로 이미 세팅했으면 스킵
	if (LastSetupInputComponent.Get() == PlayerInputComponent && bInputInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC already done for this InputComponent. Skip."));
		return;
	}

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero] SetupPIC FAIL: Owner is not Pawn. Owner=%s"), *GetNameSafe(GetOwner()));
		return;
	}

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero] SetupPIC FAIL: PC is null. Pawn=%s"), *GetNameSafe(Pawn));
		return;
	}

	// 1) 여기서 실제 입력 초기화 수행
	InitializePlayerInput(PlayerInputComponent, PC);

	// 2) 성공적으로 끝났다면 캐시/플래그 갱신
	if (bInputInitialized)
	{
		LastSetupInputComponent = PlayerInputComponent;
		UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC DONE InputComp=%s"), *GetNameSafe(PlayerInputComponent));
	}
}

void URSHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("[Hero] PlayerInputComponent=%s CastRS=%s"),
		*GetNameSafe(PlayerInputComponent),
		Cast<URSEnhancedInputComponent>(PlayerInputComponent) ? TEXT("OK") : TEXT("FAIL"));

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


	const URSInputConfig* InputConfig = Char->GetInputConfig();
	if (!InputConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero] InputConfig is NULL (skip IMC rebuild)"));
		return;
	}

	Subsystem->ClearAllMappings();

	const int32 NumContexts = InputConfig->DefaultMappings.Num();
	UE_LOG(LogTemp, Warning, TEXT("[Hero] IMC rebuild: ClearAllMappings then Add %d contexts"), NumContexts);

	for (const auto& Mapping : InputConfig->DefaultMappings)
	{
		if (UInputMappingContext* IMC = Mapping.InputMapping)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Hero] Add IMC: %s Priority=%d"),
				*GetNameSafe(IMC), Mapping.Priority);

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
	//TArray<uint32> Handles;
	//IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityTagPressed, &ThisClass::Input_AbilityTagReleased, Handles);

	// Ability 태그 바인딩
	BaseAbilityBindHandles.Reset();
	IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityTagPressed, &ThisClass::Input_AbilityTagReleased, BaseAbilityBindHandles);

	LogAbilityBindings(InputConfig, TEXT("Base"));

	const FRSGameplayTags& RSGameplayTag = FRSGameplayTags::Get();
	// Native 태그 바인딩
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	UE_LOG(LogTemp, Warning, TEXT("[Hero] BindNativeAction: Move OK"));

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	UE_LOG(LogTemp, Warning, TEXT("[Hero] BindNativeAction: Look OK"));

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Interaction,ETriggerEvent::Triggered, this, &ThisClass::Input_Interaction);
	
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_InventoryToggle, ETriggerEvent::Started, this, &ThisClass::Input_InventoryToggle);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_EquipSlot1, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipSlot1);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_EquipSlot2, ETriggerEvent::Triggered, this, &ThisClass::Input_EquipSlot2);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Interaction,ETriggerEvent::Started, this, &ThisClass::Input_Interaction);
	
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_InventoryToggle, ETriggerEvent::Started, this, &ThisClass::Input_InventoryToggle);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_QuickSlotCycle, ETriggerEvent::Started,this,&ThisClass::Input_QuickSlotCycle);
	 
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_QuickSlotUse,ETriggerEvent::Started,this,&ThisClass::Input_QuickSlotUse);



	bInputInitialized = true;
	UE_LOG(LogTemp, Warning, TEXT("[Hero] InputReady Broadcast"));
	OnInputReady.Broadcast();
}

void URSHeroComponent::Input_AbilityTagPressed(FGameplayTag InputTag)
{
	if (ARSCharacter* Char = GetOwnerCharacter())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero][Input] Pressed Tag=%s"), *InputTag.ToString());
		Char->AbilityInputTagPressed(InputTag);
	}
}

void URSHeroComponent::Input_AbilityTagReleased(FGameplayTag InputTag)
{
	if (ARSCharacter* Char = GetOwnerCharacter())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero][Input] Released Tag=%s"), *InputTag.ToString());
		Char->AbilityInputTagReleased(InputTag);
	}
}



//void URSHeroComponent::Input_AbilityTagPressed(FGameplayTag InputTag)
//{
//	ARSCharacter* Char = GetOwnerCharacter();
//	if (!Char) return;
//
//	FGameplayEventData Payload;
//	Payload.EventTag = InputTag;
//	Payload.EventMagnitude = 1.f; // Pressed일때.
//	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, InputTag, Payload);
//}
//
//void URSHeroComponent::Input_AbilityTagReleased(FGameplayTag InputTag)
//{
//	ARSCharacter* Char = GetOwnerCharacter();
//	if (!Char) return;
//
//	FGameplayEventData Payload;
//	Payload.EventTag = InputTag;
//	Payload.EventMagnitude = 0.f; // Released일떄.
//	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, InputTag, Payload);
//}

ARSCharacter* URSHeroComponent::GetOwnerCharacter() const
{
	return Cast<ARSCharacter>(GetOwner());
}

void URSHeroComponent::HandleEquipInput(FGameplayTag InputTag)
{
	if (!EquipManager)
	{
		return;
	}

	EquipManager->HandleEquipSlotInput(InputTag);
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

void URSHeroComponent::Input_EquipSlot1()
{
	if (!EquipManager)
	{
		return;
	}

	EquipManager->HandleEquipSlotInput(
		FRSGameplayTags::Get().InputTag_Native_EquipSlot1
	);
}

void URSHeroComponent::Input_EquipSlot2()
{
	if (!EquipManager)
	{
		return;
	}

	EquipManager->HandleEquipSlotInput(
		FRSGameplayTags::Get().InputTag_Native_EquipSlot2
	);
}

void URSHeroComponent::ApplyOverlayInputConfig(const URSInputConfig* OverlayConfig)
{
	if (!OverlayConfig)
	{
		ClearOverlayInputConfig();
		return;
	}

	URSEnhancedInputComponent* EIC = GetRSEnhancedInputComponent();
	if (!EIC)
	{
		return;
	}

	// 기존 Overlay 바인딩만 제거 후 새로 적용
	ClearOverlayInputConfig();

	CurrentOverlayConfig = OverlayConfig;

	// Overlay는 Ability 액션만 바인딩
	OverlayBindHandles.Reset();
	EIC->BindAbilityActions(
		OverlayConfig,
		this,
		&ThisClass::Input_AbilityTagPressed,
		&ThisClass::Input_AbilityTagReleased,
		OverlayBindHandles
	);

	UE_LOG(LogTemp, Log, TEXT("[HeroInput] Overlay applied (Ability only). Handles=%d"),
		OverlayBindHandles.Num());
}





void URSHeroComponent::ClearOverlayInputConfig()
{
	URSEnhancedInputComponent* EIC = GetRSEnhancedInputComponent();

	// Overlay 바인딩만 제거
	if (EIC)
	{
		for (uint32 Handle : OverlayBindHandles)
		{
			EIC->RemoveBindingByHandle(Handle);
		}
	}
	OverlayBindHandles.Reset();

	// IMC는 건드리지 않는다 (Base 입력 안전 보장)
	OverlayAddedIMCs.Reset();
	CurrentOverlayConfig = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[HeroInput] Overlay cleared (Ability only)."));
}



UEnhancedInputLocalPlayerSubsystem* URSHeroComponent::GetInputSubsystem() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	const APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!PC) return nullptr;

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return nullptr;

	return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}

URSEnhancedInputComponent* URSHeroComponent::GetRSEnhancedInputComponent() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return nullptr;

	return Cast<URSEnhancedInputComponent>(Pawn->InputComponent);
}

void URSHeroComponent::LogAbilityBindings(const URSInputConfig* Config, const TCHAR* Label) const
{
	if (!Config) return;

	FString Lines;
	for (const FRSInputAction& A : Config->AbilityInputActions)
	{
		if (!A.InputAction || !A.InputTag.IsValid()) continue;

		Lines += FString::Printf(TEXT("  IA=%s  Tag=%s\n"),
			*GetNameSafe(A.InputAction.Get()),
			*A.InputTag.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero][%s] AbilityInputActions:\n%s"),
		Label ? Label : TEXT("Unknown"),
		*Lines);
}

void URSHeroComponent::DebugDumpInputState(const URSInputConfig* InputConfig, const TCHAR* Label) const
{
	const TCHAR* SafeLabel = Label ? Label : TEXT("Unknown");

	UE_LOG(LogTemp, Warning, TEXT("[Hero][Dbg:%s] Owner=%s"), SafeLabel, *GetNameSafe(GetOwner()));

	if (!InputConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero][Dbg:%s] InputConfig=NULL"), SafeLabel);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero][Dbg:%s] DefaultMappings=%d"), SafeLabel, InputConfig->DefaultMappings.Num());
	for (const auto& Mapping : InputConfig->DefaultMappings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero][Dbg:%s] Config IMC=%s Priority=%d"),
			SafeLabel,
			*GetNameSafe(Mapping.InputMapping),
			Mapping.Priority);
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero][Dbg:%s] Handles: BaseAbility=%d OverlayAbility=%d"),
		SafeLabel,
		BaseAbilityBindHandles.Num(),
		OverlayBindHandles.Num());
}

void URSHeroComponent::Input_Interaction(const FInputActionValue& Value)
{
	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	Char->TryInteract();
}

void URSHeroComponent::Input_QuickSlotCycle(const FInputActionValue& Value)
{
	(void)Value;

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (ARSPlayerController* RSPC = Cast<ARSPlayerController>(Pawn->GetController()))
		{
			RSPC->QuickSlotCycle(); 
		}
	}
}

void URSHeroComponent::Input_QuickSlotUse(const FInputActionValue& Value)
{
	(void)Value;

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (ARSPlayerController* RSPC = Cast<ARSPlayerController>(Pawn->GetController()))
		{
			RSPC->QuickSlotUse();
		}
	}
}

void URSHeroComponent::Input_InventoryToggle(const FInputActionValue& Value)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (ARSPlayerController* RSPC = Cast<ARSPlayerController>(Pawn->GetController()))
		{
			RSPC->ToggleInventoryUI();
		}
	}

}



