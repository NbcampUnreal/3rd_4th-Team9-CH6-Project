#include "Character/RSCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "Component/RSWidgetComponent.h"
#include "UI/RSUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Character/RSHeroComponent.h"
#include "Input/RSEnhancedInputComponent.h"
#include "GAS/AS/RSAttributeSet_Skill.h"
#include "RSGameplayTags.h"
#include "Character/RSPawnData.h"
#include "Input/RSInputConfig.h"

// Item 관련 매니저 컴포넌트들
#include "Item/Managers/RSCosmeticManagerComponent.h"
#include "Item/Managers/RSEquipManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSItemManagerComponent.h"



//[추가]
#include "TimerManager.h"
#include "Interface/Interactable.h"
#include "DrawDebugHelpers.h"
#include "Component/Inventory/RSInventoryComponent.h"
#include "Interface/InventoryOwner.h"
#include "ItemDataAsset/RSItemData.h"

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
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/RS/UI/WBP_HPBar.WBP_HPBar_C'"));
	if (HpBarWidgetRef.Class)
	{
		HPBar->SetWidgetClass(HpBarWidgetRef.Class);
		HPBar->SetWidgetSpace(EWidgetSpace::Screen);
		HPBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HPBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName(TEXT("hand_rSocket")));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Fab/Medieval_Weapons_VOL2/Meshes/VOL2/SkeletalMesh/SK_Sword_2.SK_Sword_2'")); // /Script/Engine.SkeletalMesh'/Game/LyraResource/Weapons/Rifle/Mesh/SK_Rifle.SK_Rifle'
	if (WeaponMeshRef.Object)
	{
		WeaponMesh = WeaponMeshRef.Object;
	}

	WeaponRange = 75.f;
	WeaponAttackDamage = 100.0f;

	
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

	APlayerController* PC = Cast<APlayerController>(GetController());
	checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

	UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

	//EILPS->AddMappingContext(InputMappingContext, 0);
	
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
		//PlayerController->ConsoleCommand(TEXT("ShowDebug AbilitySystem"));
	}

	const URSAttributeSet_Character* CurrentAttributeSet = ASC->GetSet<URSAttributeSet_Character>();
	if (IsValid(CurrentAttributeSet) == true)
	{
		CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
	}

	ASC->GenericGameplayEventCallbacks.FindOrAdd(EVENT_EQUIP_WEAPON).AddUObject(this, &ThisClass::EquipWeapon);
	ASC->GenericGameplayEventCallbacks.FindOrAdd(EVENT_UNEQUIP_WEAPON).AddUObject(this, &ThisClass::UnequipWeapon);
	
	//[추가]
	UpdateInteractFocus();

	GetWorld()->GetTimerManager().SetTimer(
		InteractTraceTimer,
		this,
		&ThisClass::UpdateInteractFocus,
		InteractTraceInterval,
		true
	);
}

const URSInputConfig* ARSCharacter::GetInputConfig() const
{
	return PawnData ? PawnData->InputConfig : nullptr;
}

void ARSCharacter::OnOutOfHealth()
{
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadBody")));

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetController()->UnPossess();

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
	if (Weapon)
	{
		Weapon->SetSkeletalMesh(WeaponMesh);

		const float CurrentAttackRange = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute());
		const float CurrentAttackDamage = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute());

		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute(), CurrentAttackRange + WeaponRange);
		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute(), CurrentAttackDamage + WeaponAttackDamage);

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
	if (Weapon)
	{
		FGameplayAbilitySpec* SkillAbilitySpec = ASC->FindAbilitySpecFromClass(SkillAbilityClass);
		if (SkillAbilitySpec)
		{
			ASC->ClearAbility(SkillAbilitySpec->Handle);
		}

		const float CurrentAttackRange = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute());
		const float CurrentAttackDamage = ASC->GetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute());

		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackRangeAttribute(), CurrentAttackRange - WeaponRange);
		ASC->SetNumericAttributeBase(URSAttributeSet_Character::GetAttackDamageAttribute(), CurrentAttackDamage - WeaponAttackDamage);

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
		return;
	}

	const float Distance = FVector::Distance(
		Camera->GetComponentLocation(),
		HitActor->GetActorLocation()
	);

	if (Distance > InteractDistance)
	{
		CurrentInteractTarget = nullptr;
		CurrentInteractItemData = nullptr;
		return;
	}

	CurrentInteractTarget = HitActor;

	if (HitActor->Implements<UInteractable>())
	{
		CurrentInteractItemData =
			IInteractable::Execute_GetItemData(HitActor);
	}

	CurrentInteractTarget = HitActor;
	CurrentInteractItemData = nullptr;

	// Interactable이면 ItemData도 같이 캐싱
	if (CurrentInteractTarget && CurrentInteractTarget->Implements<UInteractable>())
	{
		CurrentInteractItemData = IInteractable::Execute_GetItemData(CurrentInteractTarget);

		if (CurrentInteractItemData)
		{
			UE_LOG(LogTemp, Log, TEXT("[Focus] %s (ItemData OK)"), *GetNameSafe(CurrentInteractTarget));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Focus] %s (No ItemData)"), *GetNameSafe(CurrentInteractTarget));
		}
	}
}



bool ARSCharacter::TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const
{
	OutActor = nullptr;

	if (!Camera) return false;
	UWorld* World = GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false);
	Params.AddIgnoredActor(this);

	// 1) 카메라 기준 트레이스 (시선 방향)
	const FVector CamStart = Camera->GetComponentLocation();
	const FVector CamDir = Camera->GetForwardVector();
	const FVector CamEnd = CamStart + CamDir * InteractTraceDistance;

	FHitResult CamHit;
	const bool bCamHit = World->LineTraceSingleByChannel(
		CamHit, CamStart, CamEnd, ECC_Visibility, Params
	);

	const FVector AimPoint = bCamHit ? CamHit.ImpactPoint : CamEnd;
	
	DrawDebugLine(
		World,
		CamStart,
		AimPoint,
		bCamHit ? FColor::Green : FColor::Red,
		false,
		0.1f,
		0,
		1.5f
	);

	if (bCamHit)
	{
		DrawDebugSphere(
			World,
			AimPoint,
			8.f,
			12,
			FColor::Green,
			false,
			0.1f
		);
	}
	// 2) 캐릭터 기준 트레이스 (실제로 닿는지)
	const FVector CharStart = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
	const FVector CharEnd = AimPoint;

	FHitResult CharHit;
	const bool bCharHit = World->LineTraceSingleByChannel(
		CharHit, CharStart, CharEnd, ECC_Visibility, Params
	);

	DrawDebugLine(
		World,
		CharStart,
		CharEnd,
		bCharHit ? FColor::Blue : FColor::Red,
		false,
		0.1f,
		0,
		1.5f
	);	
	if (!bCharHit) return false;

	OutActor = CharHit.GetActor();
	OutHit = CharHit;   
	return OutActor != nullptr;
}

bool ARSCharacter::TryAddItem_Implementation(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return false;
	}

	URSInventoryComponent* Inv = FindComponentByClass<URSInventoryComponent>();
	if (!Inv)
	{
		return false;
	}

	return Inv->AddItem(ItemData, Count);
}

bool ARSCharacter::TryRemoveItem_Implementation(URSItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return false;
	}

	URSInventoryComponent* Inv = FindComponentByClass<URSInventoryComponent>();
	if (!Inv)
	{
		return false;
	}

	return Inv->RemoveItem(ItemData, Count);
}