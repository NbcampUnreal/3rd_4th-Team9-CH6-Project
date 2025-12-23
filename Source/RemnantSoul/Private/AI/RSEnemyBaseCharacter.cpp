#include "AI/RSEnemyBaseCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
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

	/* 유효성 검사 */
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GrantAbilities(InitialAbilities);
	}
}

UAbilitySystemComponent* ARSEnemyBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> ARSEnemyBaseCharacter::GrantAbilities(
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	/* 유효성 검사 */
	if (!AbilitySystemComponent)
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	/* 부여된 어빌리티 핸들들을 저장할 배열 */
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	/* 부여할 어빌리티들을 순회하면서 GiveAbility 호출 및 핸들 저장 */
	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(Ability, 1, -1, this
		));

		AbilityHandles.Add(SpecHandle);
	}

	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void ARSEnemyBaseCharacter::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove)
{
	/* 유효성 검사 */
	if (!AbilitySystemComponent)
	{
		return ;
	}

	for (FGameplayAbilitySpecHandle AbilityHandle : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void ARSEnemyBaseCharacter::SendAbilitiesChangedEvent()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.AbilitiesChanged"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

