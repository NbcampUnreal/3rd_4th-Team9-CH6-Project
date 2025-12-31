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

void URSHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URSHeroComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (bSetupPICAcknowledged)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC already called. Skip."));
		return;
	}
	bSetupPICAcknowledged = true;

	UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC ENTER Owner=%s InputComp=%s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(PlayerInputComponent));

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

	ARSPlayerState* PS = Pawn->GetPlayerState<ARSPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("[Hero] SetupPIC FAIL: PlayerState is null. Pawn=%s"), *GetNameSafe(Pawn));
		return;
	}

	const URSHeroData* HDNow = PS->GetHeroData();
	UE_LOG(LogTemp, Warning, TEXT("[Hero] SetupPIC OK: PC=%s PS=%s HeroDataNow=%s"),
		*GetNameSafe(PC),
		*GetNameSafe(PS),
		*GetNameSafe(HDNow));

	// ---- 기존 Apply 로직 ----
	auto Apply = [this, PlayerInputComponent, PC](const URSHeroData* InHD)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Hero] Apply HeroData=%s"), *GetNameSafe(InHD));

			ARSCharacter* Char = GetOwnerCharacter();
			if (!Char || !InHD)
			{
				UE_LOG(LogTemp, Error, TEXT("[Hero] Apply FAIL: Char=%s InHD=%s"),
					*GetNameSafe(Char), *GetNameSafe(InHD));
				return;
			}

			Char->SetHeroData(InHD);
			InitializePlayerInput(PlayerInputComponent, PC);
		};

	if (HDNow)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Hero] HeroData already ready -> Apply now"));
		Apply(HDNow);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero] HeroData not ready -> bind OnHeroDataReady"));
	PS->OnHeroDataReady.AddLambda([Apply](const URSHeroData* InHD)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Hero] OnHeroDataReady fired InHD=%s"), *GetNameSafe(InHD));
			Apply(InHD);
		});
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

	Subsystem->ClearAllMappings();

	const URSInputConfig* InputConfig = Char->GetInputConfig(); 
	if (!InputConfig) return;

	if (Char->IsPlayerControlledPawn())
	{
		const URSHeroData* HD = Char->GetHeroData();
		if (!HD || !HD->PawnData)
		{
			UE_LOG(LogTemp, Error, TEXT("[Hero] Missing HeroData/PawnData on Player %s"), *GetNameSafe(Char));
			return;
		}
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
	//TArray<uint32> Handles;
	//IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityTagPressed, &ThisClass::Input_AbilityTagReleased, Handles);

	// Ability 태그 바인딩
	BaseAbilityBindHandles.Reset();
	IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityTagPressed, &ThisClass::Input_AbilityTagReleased, BaseAbilityBindHandles);

	LogAbilityBindings(InputConfig, TEXT("Base"));

	const FRSGameplayTags& RSGameplayTag = FRSGameplayTags::Get();
	// Native 태그 바인딩
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_Interaction,ETriggerEvent::Triggered, this, &ThisClass::Input_Interaction);
	
	IC->BindNativeAction(InputConfig, RSGameplayTag.InputTag_Native_InventoryToggle, ETriggerEvent::Started, this, &ThisClass::Input_InventoryToggle);

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

void URSHeroComponent::ApplyOverlayInputConfig(const URSInputConfig* Overlay)
{
	// 0) Overlay가 없으면 Clear로 수렴
	if (!Overlay)
	{
		ClearOverlayInputConfig();
		return;
	}

	// 1) 동일 Overlay면 스킵
	if (CurrentOverlayConfig == Overlay)
	{
		return;
	}

	//  (추가) Subsystem / InputComp 먼저 획득 (실패하면 기존 상태 유지)
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem();
	URSEnhancedInputComponent* EIC = GetRSEnhancedInputComponent();
	if (!Subsystem || !EIC)
	{
		return;
	}

	//  (이 위치로 이동) 기존 Overlay 정리(바인딩/IMC 모두)
	ClearOverlayInputConfig();

	// 4) Overlay IMC 추가 + “추적”
	OverlayAddedIMCs.Reset();

	for (const FRSInputMappingContextAndPriority& Mapping : Overlay->DefaultMappings)
	{
		if (!Mapping.InputMapping) continue;

		FModifyContextOptions Options;
		Options.bIgnoreAllPressedKeysUntilRelease = true; // ✅ 입력 꼬임 방지
		Options.bForceImmediately = true;

		Subsystem->AddMappingContext(Mapping.InputMapping, Mapping.Priority, Options);
		OverlayAddedIMCs.Add(Mapping.InputMapping);
	}

	//  (여기) Base Ability 바인딩 제거(중복 이벤트 방지)
	if (BaseAbilityBindHandles.Num() > 0)
	{
		for (uint32 Handle : BaseAbilityBindHandles)
		{
			EIC->RemoveBindingByHandle(Handle);
		}
		BaseAbilityBindHandles.Reset();
	}

	// 6) Overlay Ability 바인딩(핸들 추적)
	OverlayBindHandles.Reset();
	EIC->BindAbilityActions(
		Overlay,
		this,
		&ThisClass::Input_AbilityTagPressed,
		&ThisClass::Input_AbilityTagReleased,
		OverlayBindHandles
	);

	CurrentOverlayConfig = Overlay;

	// 7) 디버그 로그
	LogAbilityBindings(Overlay, TEXT("Overlay Applied"));
}



void URSHeroComponent::ClearOverlayInputConfig()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem();
	URSEnhancedInputComponent* EIC = GetRSEnhancedInputComponent();

	// 1) Overlay 바인딩 제거
	if (EIC)
	{
		for (uint32 Handle : OverlayBindHandles)
		{
			EIC->RemoveBindingByHandle(Handle);
		}
	}
	OverlayBindHandles.Reset();

	// 2) Overlay IMC 제거(“추적 배열” 기준)
	if (Subsystem)
	{
		for (const TWeakObjectPtr<UInputMappingContext>& IMCWeak : OverlayAddedIMCs)
		{
			if (UInputMappingContext* IMC = IMCWeak.Get())
			{
				Subsystem->RemoveMappingContext(IMC);
			}
		}
	}
	OverlayAddedIMCs.Reset();

	CurrentOverlayConfig = nullptr;

	// 3) Base Ability 바인딩 복구
	//    (BaseConfig는 Char->GetInputConfig()로 가져옴)
	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	const URSInputConfig* BaseConfig = Char->GetInputConfig();
	if (!BaseConfig) return;

	if (!EIC)
	{
		// InputComp가 없으면 복구 불가 (SetupPlayerInputComponent 호출 타이밍 문제)
		return;
	}

	BaseAbilityBindHandles.Reset();
	EIC->BindAbilityActions(
		BaseConfig,
		this,
		&ThisClass::Input_AbilityTagPressed,
		&ThisClass::Input_AbilityTagReleased,
		BaseAbilityBindHandles
	);

	// 4) 디버그 로그
	LogAbilityBindings(BaseConfig, TEXT("Overlay Cleared -> Base Restored"));
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

void URSHeroComponent::Input_Interaction(const FInputActionValue& Value)
{
	ARSCharacter* Char = GetOwnerCharacter();
	if (!Char) return;

	Char->TryInteract();
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

//bool ARSCharacter::IsPlayerControlledPawn() const
//{
//	const AController* C = GetController();
//	return C && C->IsPlayerController();
//}
//
//void ARSCharacter::PossessedBy(AController* NewController)
//{
//	Super::PossessedBy(NewController);
//	InitializeAbilitySystemAndPawnData();
//}
//
//void ARSCharacter::OnRep_PlayerState()
//{
//	Super::OnRep_PlayerState();
//	InitializeAbilitySystemAndPawnData();
//}
//
//void ARSCharacter::InitializeAbilitySystemAndPawnData()
//{
//	if (!ASC) return;
//
//	// ASC ActorInfo는 여기서 확정
//	ASC->InitAbilityActorInfo(this, this);
//
//	const URSPawnData* PD = GetPawnData();
//	if (!PD) return;
//
//	// (선택) 중복 부여 방지: 이미 부여했다면 스킵
//	// if (PawnGrantedAbilitySetHandles.Num() > 0) return;
//
//	PawnGrantedAbilitySetHandles.Reset();
//	PawnGrantedAbilitySetHandles.SetNum(PD->AbilitySets.Num());
//
//	for (int32 i = 0; i < PD->AbilitySets.Num(); ++i)
//	{
//		const URSAbilitySet* Set = PD->AbilitySets[i];
//		if (!Set) continue;
//
//		Set->GiveToAbilitySystem(ASC, &PawnGrantedAbilitySetHandles[i], this);
//	}
//}
