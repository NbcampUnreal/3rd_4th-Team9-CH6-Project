#include "AI/RSEnemyBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AI/RSEnemyBaseAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ARSEnemyBaseCharacter::ARSEnemyBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	/* Ability System Component 설정 */
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystemComponent->SetIsReplicated(false);

	/* AttributeSet 설정 */
	EnemyBasicAttributeSet = CreateDefaultSubobject<URSEnemyBaseAttributeSet>(TEXT("EnemyBasicAttributeSet"));

	/* 캡슐 사이즈 설정 */
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	
	/* 컨트롤러의 회전 값 사용 제어 */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* 캐릭터 무브먼트 구성 값 */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
}

void ARSEnemyBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

UAbilitySystemComponent* ARSEnemyBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

