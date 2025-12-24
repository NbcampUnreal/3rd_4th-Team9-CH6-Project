#include "Character/RSBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "Character/Ability/RSAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/PlayerController/RSPlayerController.h"


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

		return;
	}
	

	/* 2. 실제 활성화 시도 */
	AbilitySystem->TryActivateAbilitiesByTag(TagContainer);

	/* 3. 활성화 결과 확인 */
	for (const FGameplayAbilitySpec* Spec : MatchingSpecs)
	{
		const bool bIsActive = Spec->IsActive();

	}
}

void ARSBaseCharacter::RollInput()
{
	if (bIsDead || !AbilitySystem)
	{

		return;
	}

	const FGameplayTag RollTag =
		FGameplayTag::RequestGameplayTag(TEXT("Ability.Roll"));

	FGameplayTagContainer TagContainer(RollTag);

	/* 1. 해당 태그를 가진 어빌리티가 있는지 확인 */
	TArray<FGameplayAbilitySpec*> MatchingSpecs;
	AbilitySystem->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		TagContainer,
		MatchingSpecs,
		false
	);

	if (MatchingSpecs.Num() == 0)
	{

		return;
	}



	/* 2. 실제 활성화 시도 */
	AbilitySystem->TryActivateAbilitiesByTag(TagContainer);

	/* 3. 활성화 결과 확인 */
	for (const FGameplayAbilitySpec* Spec : MatchingSpecs)
	{
		const bool bIsActive = Spec->IsActive();
		
	}
}


void ARSBaseCharacter::CrouchInput_Pressed()
{
	if (bIsDead || !AbilitySystem)
	{

		return;
	}

	const FGameplayTag CrouchTag =
		FGameplayTag::RequestGameplayTag(TEXT("Ability.Crouch"));

	FGameplayTagContainer TagContainer(CrouchTag);

	TArray<FGameplayAbilitySpec*> MatchingSpecs;
	AbilitySystem->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		TagContainer,
		MatchingSpecs,
		false
	);

	if (MatchingSpecs.Num() == 0)
	{
		return;
	}
	
	AbilitySystem->TryActivateAbilitiesByTag(TagContainer);
	
}

void ARSBaseCharacter::CrouchInput_Released()
{
	if (!AbilitySystem)
	{
		return;
	}

	const FGameplayTag EndRequestTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Crouch.EndRequested"));

	FGameplayEventData Payload;
	Payload.EventTag = EndRequestTag;
	Payload.Instigator = this;


	AbilitySystem->HandleGameplayEvent(EndRequestTag, &Payload);
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
			continue;
		}


		AbilitySystem->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	}

	bAbilitiesInitialized = true;
}

