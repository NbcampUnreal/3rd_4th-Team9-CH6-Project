#include "Character/RSCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"

#include "GAS/AS/RSAttributeSet_Character.h"
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

// Animation
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

// Interaction / Inventory
#include "TimerManager.h"
#include "Interface/Interactable.h"
#include "DrawDebugHelpers.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "Interface/InventoryOwner.h"
#include "ItemDataAsset/RSItemData.h"

// HPBar 위젯 로드에 필요한 헤더(프로젝트 환경에 따라 간접 포함이 안 될 수 있어 명시)
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

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

	// Weapon 컴포넌트를 실제로 쓸 거면 여기 CreateDefaultSubobject를 켜야 런타임에서 null이 아님.
	// (컴파일에는 영향 없음)
	// Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	// Weapon->SetupAttachment(GetMesh(), FName(TEXT("hand_rSocket")));

	Inventory = CreateDefaultSubobject<URSInventoryComponent>(TEXT("Inventory"));
}

void ARSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsValid(HeroComponent))
	{
		HeroComponent->SetupPlayerInputComponent(PlayerInputComponent);
	}
}

void ARSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Player 전용 체크(기존 코드가 Player가 아닐 때 크래시 낼 수 있어서 최소 방어)
	if (IsPlayerControlledPawn())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));
	}

	// 기존처럼 BeginPlay에서 ActorInfo 초기화(네 코드 유지)
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
	}

	// PawnData 기반 기본 AbilitySets 부여 (기존 동작 유지)
	//    단, PossessedBy/OnRep에서 또 줄 수도 있으므로 "중복 부여"만 최소 차단
	InitializeAbilitySystemAndPawnData();

	UE_LOG(LogTemp, Warning, TEXT("[ASC] Activatable=%d"), ASC ? ASC->GetActivatableAbilities().Num() : -1);

	if (ASC)
	{
		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ASC] Spec=%s Tags=%s"),
				*GetNameSafe(Spec.Ability),
				*Spec.DynamicAbilityTags.ToString());
		}
	}

	// OutOfHealth 델리게이트
	if (ASC)
	{
		const URSAttributeSet_Character* CurrentAttributeSet = ASC->GetSet<URSAttributeSet_Character>();
		if (IsValid(CurrentAttributeSet))
		{
			CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}
	}

	// Equip/Unequip 이벤트 콜백(기존 유지)
	if (ASC)
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EVENT_EQUIP_WEAPON).AddUObject(this, &ThisClass::EquipWeapon);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EVENT_UNEQUIP_WEAPON).AddUObject(this, &ThisClass::UnequipWeapon);
	}

	// Interaction 타이머(기존 유지)
	UpdateInteractFocus();

	GetWorld()->GetTimerManager().SetTimer(
		InteractTraceTimer,
		this,
		&ThisClass::UpdateInteractFocus,
		InteractTraceInterval,
		true
	);
}

void ARSCharacter::OnOutOfHealth()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadBody")));

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

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

		// 기존 InputID 방식 유지(네 코드 그대로)
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

void ARSCharacter::UpdateInteractFocus()
{
	AActor* HitActor = nullptr;
	FHitResult Hit;

	if (!TraceInteractTarget(HitActor, Hit))
	{
		CurrentInteractTarget = nullptr;
		CurrentInteractItemData = nullptr;
		CurrentInteractHit = FHitResult();
		return;
	}

	// 같은 대상이면 Hit만 갱신하고 끝
	if (CurrentInteractTarget == HitActor)
	{
		CurrentInteractHit = Hit;
		return;
	}

	CurrentInteractTarget = HitActor;
	CurrentInteractHit = Hit;
	CurrentInteractItemData = nullptr;

	if (CurrentInteractTarget && CurrentInteractTarget->Implements<UInteractable>())
	{
		CurrentInteractItemData = IInteractable::Execute_GetItemData(CurrentInteractTarget);
	}
}

void ARSCharacter::TryInteract()
{
	if (!CurrentInteractTarget) return;
	if (!CurrentInteractTarget->Implements<UInteractable>()) return;
	if (!Camera) return;

	const float Distance = FVector::Distance(Camera->GetComponentLocation(), CurrentInteractHit.ImpactPoint);

	DrawDebugString(
		GetWorld(),
		CurrentInteractTarget->GetActorLocation() + FVector(0, 0, 40),
		FString::Printf(TEXT("%.0f cm"), Distance),
		nullptr,
		FColor::White,
		0.1f
	);

	if (Distance > InteractDistance) return;

	if (IInteractable::Execute_CanInteract(CurrentInteractTarget, this))
	{
		IInteractable::Execute_Interact(CurrentInteractTarget, this);
	}
}

bool ARSCharacter::TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const
{
	OutActor = nullptr;
	OutHit = FHitResult();

	if (!Camera) return false;
	UWorld* World = GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false);
	Params.AddIgnoredActor(this);

	// 1) 카메라 시선 트레이스
	const FVector CamStart = Camera->GetComponentLocation();
	const FVector CamDir = Camera->GetForwardVector();
	const FVector CamEnd = CamStart + CamDir * InteractTraceDistance;

	FHitResult CamHit;
	const bool bCamHit = World->LineTraceSingleByChannel(CamHit, CamStart, CamEnd, ECC_Visibility, Params);
	const FVector AimPoint = bCamHit ? CamHit.ImpactPoint : CamEnd;

	// 2) 캐릭터에서 AimPoint로 트레이스
	const FVector CharStart = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	const FVector CharEnd = AimPoint;

	FHitResult CharHit;
	const bool bCharHit = World->LineTraceSingleByChannel(CharHit, CharStart, CharEnd, ECC_Visibility, Params);
	if (!bCharHit) return false;

	OutActor = CharHit.GetActor();
	OutHit = CharHit;
	return (OutActor != nullptr);
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
	if (!InputTag.IsValid()) return;
	if (!ASC) return;

	FScopedAbilityListLock Lock(*ASC);

	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			ASC->AbilitySpecInputPressed(Spec);

			if (!Spec.IsActive())
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
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

	// ActorInfo 보장(여러 군데서 호출돼도 안전)
	ASC->InitAbilityActorInfo(this, this);

	const URSPawnData* PD = GetPawnData();
	if (!PD) return;

	// 중복 부여 최소 차단 (BeginPlay + PossessedBy + OnRep 에서 여러 번 들어와도 안전)
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
}
