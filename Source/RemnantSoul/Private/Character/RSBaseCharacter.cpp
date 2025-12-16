#include "Character/RSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Character/Ability/RSAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/PlayerController/RSPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRSAbility, Log, All);

ARSBaseCharacter::ARSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Camera */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 12.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	/* GAS */
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(false); // 싱글 RPG

	AttributeSet = CreateDefaultSubobject<URSAttributeSet>(TEXT("AttributeSet"));

	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

UAbilitySystemComponent* ARSBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ARSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	AbilitySystem->InitAbilityActorInfo(this, this);
	
	InitializeAbilities();


}


void ARSBaseCharacter::HandleMoveInput(const FVector2D& MoveVector)
{
	if (bIsDead || MoveVector.IsNearlyZero())
	{
		return;
	}

	// 컨트롤러 유효성 체크
	if (!Controller)
	{
		return;
	}

	// 컨트롤러 회전값에서 Yaw만 사용
	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	// 이동 기준 벡터 계산
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 입력값 기반 이동 처리
	AddMovementInput(ForwardDirection, MoveVector.Y);
	AddMovementInput(RightDirection,   MoveVector.X);
	
}

void ARSBaseCharacter::HandleLookInput(const FVector2D& LookVector)
{
	if (bIsDead)
	{
		return;
	}

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void ARSBaseCharacter::HandleAttackInput()
{
	if (bIsDead || !AbilitySystem)
	{
		UE_LOG(LogRSAbility, Warning,
			TEXT("[AttackInput] Blocked - Dead: %d, ASC Valid: %d"),
			bIsDead,
			AbilitySystem != nullptr
		);
		return;
	}

	const FGameplayTag AttackTag =
		FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack"));

	FGameplayTagContainer TagContainer(AttackTag);

	/* 1. 해당 태그를 가진 어빌리티가 있는지 확인 */
	TArray<FGameplayAbilitySpec*> MatchingSpecs;
	AbilitySystem->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		TagContainer,
		MatchingSpecs,
		false
	);

	if (MatchingSpecs.Num() == 0)
	{
		UE_LOG(LogRSAbility, Warning,
			TEXT("[AttackInput] No Ability found with tag: %s"),
			*AttackTag.ToString()
		);
		return;
	}

	UE_LOG(LogRSAbility, Log,
		TEXT("[AttackInput] Found %d Ability(s) with tag: %s"),
		MatchingSpecs.Num(),
		*AttackTag.ToString()
	);

	/* 2. 실제 활성화 시도 */
	AbilitySystem->TryActivateAbilitiesByTag(TagContainer);

	/* 3. 활성화 결과 확인 */
	for (const FGameplayAbilitySpec* Spec : MatchingSpecs)
	{
		const bool bIsActive = Spec->IsActive();

		UE_LOG(LogRSAbility, Log,
			TEXT("[AttackInput] Ability %s | Active: %d"),
			*GetNameSafe(Spec->Ability),
			bIsActive
		);
	}
}


/* ===== GAS ===== */

void ARSBaseCharacter::InitializeAbilities()
{
	if (!AbilitySystem || bAbilitiesInitialized)
	{
		return;
	}
	

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (!AbilityClass)
		{
			UE_LOG(LogRSAbility, Warning,
				TEXT("[GAS] AbilityClass is NULL"));
			continue;
		}

		UE_LOG(LogRSAbility, Log,TEXT("[GAS] Giving Ability: %s"),*AbilityClass->GetName());

		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	}

	bAbilitiesInitialized = true;
}


void ARSBaseCharacter::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	UE_LOG(LogRSAbility, Log, TEXT("[Death] Character died"));

	// 캐릭터 이동 차단
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 마우스 회전 차단
	bUseControllerRotationYaw = false;

	// GAS 처리
	if (AbilitySystem)
	{
		AbilitySystem->CancelAllAbilities();
		bAbilitiesInitialized = false;
		//추후 태그 추가 후 주석 해제 필요.
		//AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
	}

	//입력 비활성 (캐릭터 기준)
	DisableInput(nullptr);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ARSPlayerController* RSPC = Cast<ARSPlayerController>(PC))
		{
			RSPC->OnPlayerDeath();
			Destroy();
			//TODO: 캐릭터 삭제 코드는 애니메이션 작업에 따라 사망 애니메이션 출력으로 수정 필요
		}
	}
}

