#include "Character/RSCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/PlayerController/RSPlayerController.h"

#include "GAS/AS/RSAttributeSet_Character.h"
#include "GAS/AS/RSAttributeSet_Stamina.h"
#include "GAS/AS/RSAttributeSet_Skill.h"
#include "GAS/AS/RSAbilitySet.h"

#include "Component/RSWidgetComponent.h"
#include "UI/RSUserWidget.h"
#include "Components/CapsuleComponent.h"

#include "Character/RSHeroComponent.h"
#include "Input/RSEnhancedInputComponent.h"
#include "RSGameplayTags.h"

#include "Character/RSPawnData.h"
#include "Input/RSInputConfig.h"
#include "Character/RSHeroData.h"
#include "Character/RSCombatStyleData.h"

// Item 관련 매니저 컴포넌트들
#include "Item/Managers/RSCosmeticManagerComponent.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"
#include "Interface/RSItemSource.h"
#include "Item/RSItemTemplate.h"
#include "Item/RSItemInstance.h"
#include "Item/RSWeaponPickupActor.h"

// Animation
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

// Interaction / Inventory
#include "TimerManager.h"
#include "Interface/Interactable.h"
#include "DrawDebugHelpers.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "ItemDataAsset/RSItemData.h"


// HPBar 위젯 로드에 필요한 헤더(프로젝트 환경에 따라 간접 포함이 안 될 수 있어 명시)
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GAS/GA/RSGamePlayAbility_Interact.h"

#include "HAL/IConsoleManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogRSInteract, Log, All);

static TAutoConsoleVariable<int32> CVarInteractDebug(
	TEXT("rs.InteractDebug"),
	0,
	TEXT("0: off, 1: on"),
	ECVF_Default
);
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

	HeroComponent = CreateDefaultSubobject<URSHeroComponent>(TEXT("HeroComponent"));
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<URSAttributeSet_Character>(TEXT("AttributeSet"));
	AttributeSet_Stamina = CreateDefaultSubobject<URSAttributeSet_Stamina>(TEXT("StaminaSet"));
	SkillAttributeSet = CreateDefaultSubobject<URSAttributeSet_Skill>(TEXT("SkillAttributeSet"));

	// Item 관련 매니저 컴포넌트들
	InventoryManager = CreateDefaultSubobject<URSInventoryManagerComponent>(TEXT("InventoryManager"));
	EquipmentManager = CreateDefaultSubobject<URSEquipmentManagerComponent>(TEXT("EquipmentManager"));
	EquipManager = CreateDefaultSubobject<URSEquipManagerComponent>(TEXT("EquipManager"));
	CosmeticManager = CreateDefaultSubobject<URSCosmeticManagerComponent>(TEXT("CosmeticManager"));

	HPBar = CreateDefaultSubobject<URSWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(GetMesh());
	HPBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));

	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/RS/UI/WBP_HPBar.WBP_HPBar_C'")
	);

	if (HpBarWidgetRef.Class)
	{
		HPBar->SetWidgetClass(HpBarWidgetRef.Class);
		HPBar->SetWidgetSpace(EWidgetSpace::Screen);
		HPBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HPBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}



	Inventory = CreateDefaultSubobject<URSInventoryComponent>(TEXT("InventoryComponent"));

	UE_LOG(LogTemp, Warning, TEXT("[Char] BeginPlay Pawn=%s Controlled=%d Controller=%s"),
		*GetNameSafe(this),
		IsPlayerControlledPawn() ? 1 : 0,
		*GetNameSafe(GetController()));
}

void ARSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> Comps = K2_GetComponentsByClass(URSHeroComponent::StaticClass());

	UE_LOG(LogTemp, Warning, TEXT("[HeroComp] Num=%d"), Comps.Num());
	for (UActorComponent* C : Comps)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HeroComp]  - %s"), *GetPathNameSafe(C));
	}

	if (IsPlayerControlledPawn())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));
	}

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
	}

	InitializeAbilitySystemAndPawnData();


	if (ASC)
	{
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ASC] Spec=%s Tags=%s"),
				*GetNameSafe(Spec.Ability),
				*Spec.DynamicAbilityTags.ToString());
		}
	}

	if (ASC)
	{
		const URSAttributeSet_Character* CurrentAttributeSet = ASC->GetSet<URSAttributeSet_Character>();
		if (IsValid(CurrentAttributeSet))
		{
			CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}
	}

	if (ASC)
	{
		const FRSGameplayTags& RSTags = FRSGameplayTags::Get();
		ASC->GenericGameplayEventCallbacks.FindOrAdd(RSTags.Event_Equip_Weapon).AddUObject(this, &ThisClass::EquipWeapon);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(RSTags.Event_Unequip_Weapon).AddUObject(this, &ThisClass::UnequipWeapon);
	}

	UpdateInteractFocus();

	GetWorld()->GetTimerManager().SetTimer(
		InteractTraceTimer,
		this,
		&ThisClass::UpdateInteractFocus,
		InteractTraceInterval,
		true
	);
	
	if (ASC)
	{
		const FGameplayTag InteractTag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Interact"));

		int32 Count = 0;
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.DynamicAbilityTags.HasTagExact(InteractTag))
			{
				++Count;
				UE_LOG(LogTemp, Warning, TEXT("[ASC] InteractTagSpec Handle=%s Class=%s Tags=%s"),
					*Spec.Handle.ToString(),
					*GetNameSafe(Spec.Ability),
					*Spec.DynamicAbilityTags.ToString());
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("[ASC] InteractTag COUNT = %d"), Count);
	}
}

void ARSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("[Char] SetupPIC Pawn=%s HeroComp=%s Valid=%d"),
		*GetNameSafe(this),
		*GetNameSafe(HeroComponent),
		IsValid(HeroComponent) ? 1 : 0);

	if (!IsValid(HeroComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("[Char] HeroComponent is NULL/Invalid. Check CreateDefaultSubobject name/UPROPERTY."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Char] Calling HeroComp->SetupPIC"));
	HeroComponent->SetupPlayerInputComponent(PlayerInputComponent);
	
	static int32 SetupCount = 0;
	UE_LOG(LogTemp, Warning, TEXT("[SetupPIC] count=%d frame=%llu this=%s"),
		++SetupCount, (unsigned long long)GFrameCounter, *GetPathNameSafe(this));
}

void ARSCharacter::OnOutOfHealth()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadBody")));

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	if (ARSPlayerController* PC = Cast<ARSPlayerController>(GetController()))
	{
		PC->ShowGameOverUI();
	}

	if (AController* C = GetController())
	{
		C->UnPossess();
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}
}

UAbilitySystemComponent* ARSCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARSCharacter::EquipWeapon(const FGameplayEventData* EventData)
{
	if (!ASC) return;

	if (Weapon)
	{
		Weapon->SetSkeletalMesh(WeaponMesh);

		const float CurrentAttackRange =
			ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute());

		const float CurrentAttackDamage =
			ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute());

		ASC->SetNumericAttributeBase(
			URSAttributeSet_Character::GetAttackRangeAttribute(),
			CurrentAttackRange + WeaponRange
		);

		ASC->SetNumericAttributeBase(
			URSAttributeSet_Character::GetAttackDamageAttribute(),
			CurrentAttackDamage + WeaponAttackDamage
		);

		
		FGameplayAbilitySpec NewSkillSpec(SkillAbilityClass);
		NewSkillSpec.InputID = 3;

		if (!ASC->FindAbilitySpecFromClass(SkillAbilityClass))
		{
			ASC->GiveAbility(NewSkillSpec);
		}
	}
}

void ARSCharacter::UnequipWeapon(const FGameplayEventData* EventData)
{
	if (!ASC) return;

	if (Weapon)
	{
		if (FGameplayAbilitySpec* SkillAbilitySpec = ASC->FindAbilitySpecFromClass(SkillAbilityClass))
		{
			ASC->ClearAbility(SkillAbilitySpec->Handle);
		}

		const float CurrentAttackRange =
			ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute());

		const float CurrentAttackDamage =
			ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute());

		ASC->SetNumericAttributeBase(
			URSAttributeSet_Character::GetAttackRangeAttribute(),
			CurrentAttackRange - WeaponRange
		);

		ASC->SetNumericAttributeBase(
			URSAttributeSet_Character::GetAttackDamageAttribute(),
			CurrentAttackDamage - WeaponAttackDamage
		);

		Weapon->SetSkeletalMesh(nullptr);
	}
}



bool ARSCharacter::TryAddItem_Implementation(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0) return false;

	URSInventoryComponent* Inv = FindComponentByClass<URSInventoryComponent>();
	if (!Inv) return false;

	return Inv->AddItem(ItemData, Count);
}

bool ARSCharacter::TryRemoveItem_Implementation(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0) return false;

	URSInventoryComponent* Inv = FindComponentByClass<URSInventoryComponent>();
	if (!Inv) return false;

	return Inv->RemoveItem(ItemData, Count);
}

void ARSCharacter::OnCombatStyleChanged(const URSCombatStyleData* NewStyle)
{
	CurrentAnimLayerTag = NewStyle ? NewStyle->AnimLayerTag : FGameplayTag();
	CurrentAnimStyleTags = NewStyle ? NewStyle->AnimStyleTags : FGameplayTagContainer();

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return;

	if (NewStyle && NewStyle->LinkedAnimLayerClass)
	{
		AnimInst->LinkAnimClassLayers(NewStyle->LinkedAnimLayerClass);
	}
}

void ARSCharacter::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[Char][Ability] Pressed Tag=%s"), *InputTag.ToString());

	DebugLogAbilityMatchesForInputTag(InputTag);

	if (!InputTag.IsValid())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Char][Ability] Invalid InputTag"));
		return;
	}

	if (!ASC)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Char][Ability] ASC is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[Char][Ability] Pressed Tag=%s | Activatable=%d"),
		*InputTag.ToString(),
		ASC->GetActivatableAbilities().Num());

	FScopedAbilityListLock Lock(*ASC);

	bool bFound = false;

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("  [Spec] %s | Tags=%s"),
			*GetNameSafe(Spec.Ability),
			*Spec.DynamicAbilityTags.ToString());

		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			bFound = true;

			UE_LOG(LogTemp, Warning,
				TEXT("  [Match] Activating %s"),
				*GetNameSafe(Spec.Ability));

			ASC->AbilitySpecInputPressed(Spec);

			if (!Spec.IsActive())
			{
				const bool bActivated = ASC->TryActivateAbility(Spec.Handle);
				UE_LOG(LogTemp, Warning,
					TEXT("  [TryActivate] Result=%s"),
					bActivated ? TEXT("SUCCESS") : TEXT("FAIL"));
			}
		}
	}

	if (!bFound)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Char][Ability] No matching AbilitySpec for Tag=%s"),
			*InputTag.ToString());
	}
}


void ARSCharacter::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	if (!ASC) return;

	FScopedAbilityListLock Lock(*ASC);

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			ASC->AbilitySpecInputReleased(Spec);
		}
	}
}

bool ARSCharacter::IsPlayerControlledPawn() const
{
	const AController* C = GetController();
	return (C && C->IsPlayerController());
}

const URSPawnData* ARSCharacter::GetPawnData() const
{
	// Player는 HeroData SSOT 강제
	if (IsPlayerControlledPawn())
	{
		const URSPawnData* PD = (HeroData ? HeroData->PawnData : nullptr);
		ensureMsgf(PD, TEXT("Player must have HeroData->PawnData. Pawn=%s"), *GetNameSafe(this));
		return PD;
	}

	// AI/임시 Pawn은 허용(선택)
	return DebugPawnDataFallback;
}

const URSInputConfig* ARSCharacter::GetInputConfig() const
{
	if (const URSPawnData* PD = GetPawnData())
	{
		return PD->InputConfig;
	}
	return nullptr;
}

void ARSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(LogTemp, Warning, TEXT("[Char] PossessedBy Pawn=%s NewController=%s IsPlayer=%d"),
		*GetNameSafe(this),
		*GetNameSafe(NewController),
		(NewController && NewController->IsPlayerController()) ? 1 : 0);

	InitializeAbilitySystemAndPawnData();
}

void ARSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystemAndPawnData();
}

void ARSCharacter::InitializeAbilitySystemAndPawnData()
{
	if (!ASC) return;

	ASC->InitAbilityActorInfo(this, this);

	// Player는 HeroData SSOT 강제
	if (!HasValidPlayerHeroData())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RS] Player HeroData/PawnData not ready. Skip Init. Pawn=%s"), *GetNameSafe(this));
		return;
	}

	const URSPawnData* PD = GetPawnData();
	if (!PD) return;

	if (PawnGrantedAbilitySetHandles.Num() > 0)
	{
		return;
	}

	PawnGrantedAbilitySetHandles.Reset();
	PawnGrantedAbilitySetHandles.SetNum(PD->AbilitySets.Num());

	for (int32 i = 0; i < PD->AbilitySets.Num(); ++i)
	{
		const URSAbilitySet* Set = PD->AbilitySets[i];
		if (!Set) continue;

		Set->GiveToAbilitySystem(ASC, &PawnGrantedAbilitySetHandles[i], this);
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[ASC][Init] Activatable=%d"),
		ASC->GetActivatableAbilities().Num());

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[ASC][Init] Spec=%s | Tags=%s"),
			*GetNameSafe(Spec.Ability),
			*Spec.DynamicAbilityTags.ToString());
	}
}

void ARSCharacter::SetHeroData(const URSHeroData* InHeroData)
{
	HeroData = const_cast<URSHeroData*>(InHeroData);

	// Player라면, HeroData 주입 시점이 "SSOT 고정점"이므로
	// 여기서 한번 더 초기화를 시도해도 안전하게 만들면 좋다.
	InitializeAbilitySystemAndPawnData();
}

bool ARSCharacter::HasValidPlayerHeroData() const
{
	if (!IsPlayerControlledPawn())
	{
		return true;
	}

	return (HeroData && HeroData->PawnData);
}

UInputComponent* ARSCharacter::CreatePlayerInputComponent()
{
	// URSEnhancedInputComponent를 Pawn의 InputComponent로 생성
	URSEnhancedInputComponent* NewIC = NewObject<URSEnhancedInputComponent>(
		this,
		URSEnhancedInputComponent::StaticClass(),
		TEXT("RSInputComponent")
	);

	// APawn::SetupPlayerInputComponent 흐름에서 사용할 수 있도록 등록
	NewIC->RegisterComponent();

	// Pawn의 InputComponent 포인터도 갱신(안전)
	InputComponent = NewIC;

	UE_LOG(LogTemp, Warning, TEXT("[Input] CreatePlayerInputComponent -> %s"),
		*GetNameSafe(InputComponent));

	return NewIC;
}

void ARSCharacter::HandleAnimEquipAction(ERSAnimEquipAction Action)
{
	if (EquipmentManager)
	{
		EquipmentManager->HandleEquipAnimAction(Action);
	}
}

URSItemInstance* ARSCharacter::GetEquippedWeaponByInputTag(FGameplayTag InputTag) const
{
	const URSEquipmentManagerComponent* EqMgr =
		FindComponentByClass<URSEquipmentManagerComponent>();

	if (!EqMgr)
	{
		return nullptr;
	}

	const FRSGameplayTags& RSTags = FRSGameplayTags::Get();

	FGameplayTag SlotTag;
	if (InputTag == RSTags.InputTag_Native_EquipSlot1)
	{
		SlotTag = RSTags.Slot_Weapon_Main;
	}
	else if (InputTag == RSTags.InputTag_Native_EquipSlot2)
	{
		SlotTag = RSTags.Slot_Weapon_Sub;
	}
	else
	{
		return nullptr;
	}

	return EqMgr->GetItemInSlot(SlotTag);
}

bool ARSCharacter::PickupWeaponTemplate(URSItemTemplate* WeaponTemplate, int32 Count, bool bAutoEquip)
{
	if (!WeaponTemplate || Count <= 0)
	{
		return false;
	}

	URSEquipmentManagerComponent* Eq = FindComponentByClass<URSEquipmentManagerComponent>();
	if (!Eq)
	{
		return false;
	}

	URSItemInstance* NewInst = NewObject<URSItemInstance>(this);
	if (!NewInst)
	{
		return false;
	}

	// Initialize는 (Template, Count, OwningActor) 3개 인수다
	NewInst->InitializeFromTemplate(WeaponTemplate, Count, this);

	UE_LOG(LogTemp, Warning, TEXT("[Pickup] Template=%s Count=%d"),
		*GetNameSafe(WeaponTemplate), Count);

	return Eq->TryPickupWeaponToSlots(NewInst, bAutoEquip);
}

bool ARSCharacter::PickupFromActor(ARSWeaponPickupActor* Pickup, bool bAutoEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("[Char] PickupFromActor ENTER Pickup=%s AutoEquip=%d"),
		*GetNameSafe(Pickup), bAutoEquip);

	if (!Pickup) return false;

	const URSItemTemplate* Template = Pickup->GetPickupTemplate();
	UE_LOG(LogTemp, Warning, TEXT("[Char] Pickup Template=%s"), *GetNameSafe(Template));
	if (!Template) return false;

	URSInventoryManagerComponent* Inv = FindComponentByClass<URSInventoryManagerComponent>();
	URSEquipmentManagerComponent* Eq = FindComponentByClass<URSEquipmentManagerComponent>();

	UE_LOG(LogTemp, Warning, TEXT("[Char] Inv=%s Eq=%s"),
		*GetNameSafe(Inv), *GetNameSafe(Eq));

	if (!Inv || !Eq) return false;

	URSItemInstance* NewInst = Inv->CreateItemInstance(Template, 1, this);
	UE_LOG(LogTemp, Warning, TEXT("[Char] CreateItemInstance NewInst=%s"), *GetNameSafe(NewInst));
	if (!NewInst) return false;

	const bool bOK = Eq->TryPickupWeaponToSlots(NewInst, bAutoEquip);
	UE_LOG(LogTemp, Warning, TEXT("[Char] TryPickupWeaponToSlots result=%s"), bOK ? TEXT("TRUE") : TEXT("FALSE"));

	if (bOK)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Char] Destroy pickup actor"));
		Pickup->Destroy();
	}
	return bOK;
}

void ARSCharacter::UpdateInteractFocus()
{
	AActor* HitActor = nullptr;
	FHitResult Hit;

	const bool bHasHit = TraceInteractTarget(HitActor, Hit);

	// 1) 못 맞췄으면: 기존 포커스 해제
	if (!bHasHit || !IsValid(HitActor))
	{
		if (IsValid(CurrentInteractTarget) && CurrentInteractTarget->Implements<UInteractable>())
		{
			IInteractable::Execute_OnFocusEnd(CurrentInteractTarget, this);
		}

		CurrentInteractTarget = nullptr;
		CurrentInteractItemData = nullptr;
		CurrentInteractHit = FHitResult();
		return;
	}

	// 2) 같은 타겟이면 히트만 갱신
	if (CurrentInteractTarget == HitActor)
	{
		CurrentInteractHit = Hit;
		return;
	}

	// 3) 타겟이 바뀌었으면: 이전 포커스 종료
	if (IsValid(CurrentInteractTarget) && CurrentInteractTarget->Implements<UInteractable>())
	{
		IInteractable::Execute_OnFocusEnd(CurrentInteractTarget, this);
	}

	// 4) 새 포커스 시작
	CurrentInteractTarget = HitActor;
	CurrentInteractHit = Hit;

	if (IsValid(CurrentInteractTarget) && CurrentInteractTarget->Implements<UInteractable>())
	{
		IInteractable::Execute_OnFocusBegin(CurrentInteractTarget, this);
	}

	// 5) 아이템 데이터 캐시(있으면)
	CurrentInteractItemData = nullptr;
	if (IsValid(CurrentInteractTarget) && CurrentInteractTarget->Implements<URSItemSource>())
	{
		CurrentInteractItemData = IRSItemSource::Execute_GetItemData(CurrentInteractTarget);
	}
}

bool ARSCharacter::TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const
{
	OutActor = nullptr;
	OutHit = FHitResult();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->PlayerCameraManager) return false;

	const FVector Start = PC->PlayerCameraManager->GetCameraLocation();
	const FRotator Rot  = PC->PlayerCameraManager->GetCameraRotation();
	const FVector End   = Start + Rot.Vector() * InteractTraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(RSInteractTrace), /*bTraceComplex*/ false);
	Params.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	if (!World) return false;

	const bool bHit = World->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit) return false;

	AActor* HitActor = OutHit.GetActor();
	if (!IsValid(HitActor)) return false;
	if (!HitActor->Implements<UInteractable>()) return false;

	OutActor = HitActor;
	return true;
}

void ARSCharacter::DebugLogAbilityMatchesForInputTag(const FGameplayTag& InputTag) const
{
	const UAbilitySystemComponent* LocalASC = FindComponentByClass<UAbilitySystemComponent>();
	if (!LocalASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Char][AbilityDbg] ASC missing. Owner=%s"), *GetNameSafe(this));
		return;
	}

	// UE 버전/프로젝트 구성에 따라 반환 타입이 TArray인 경우가 흔함
	const TArray<FGameplayAbilitySpec>& Specs = LocalASC->GetActivatableAbilities();

	int32 MatchCount = 0;

	for (const FGameplayAbilitySpec& Spec : Specs)
	{
		if (!Spec.Ability)
		{
			continue;
		}

		// RS 규칙: AbilitySet이 Spec.DynamicAbilityTags에 InputTag를 주입해야 함
		const bool bMatch = Spec.DynamicAbilityTags.HasTagExact(InputTag);
		if (!bMatch)
		{
			continue;
		}

		++MatchCount;

		UE_LOG(LogTemp, Warning,
			TEXT("  [MatchSpec] Ability=%s Level=%d InputTag=%s DynamicTags=%s"),
			*GetNameSafe(Spec.Ability),
			Spec.Level,
			*InputTag.ToString(),
			*Spec.DynamicAbilityTags.ToStringSimple());
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[Char][AbilityDbg] InputTag=%s MatchedSpecs=%d Activatable=%d"),
		*InputTag.ToString(),
		MatchCount,
		Specs.Num());
}
