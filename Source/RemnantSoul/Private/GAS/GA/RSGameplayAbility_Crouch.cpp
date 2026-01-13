#include "GAS/GA/RSGameplayAbility_Crouch.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "RSGameplayTags.h"
#include "GAS/AT/RSAbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Crouch)

URSGameplayAbility_Crouch::URSGameplayAbility_Crouch()
{
	// 태그는 프로젝트 네이밍에 맞춰 수정
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	AbilityTags.AddTag(Tags.Ability_Crouch);
	ActivationOwnedTags.AddTag(Tags.Status_Crouch);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Crouch::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CanCrouchNow())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (ACharacter* Char = GetCharacterFromActorInfo())
	{
		Char->Crouch();
	}

	if (URSAbilityTask_WaitForTick* TickTask = URSAbilityTask_WaitForTick::WaitForTick(this))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}
}

void URSGameplayAbility_Crouch::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (ACharacter* Char = GetCharacterFromActorInfo())
	{
		Char->UnCrouch();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URSGameplayAbility_Crouch::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void URSGameplayAbility_Crouch::OnTick(float DeltaTime)
{
	if (!CanCrouchNow())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

bool URSGameplayAbility_Crouch::CanCrouchNow() const
{
	ACharacter* Char = GetCharacterFromActorInfo();
	if (!Char)
	{
		return false;
	}

	UCharacterMovementComponent* Move = Char->GetCharacterMovement();
	if (!Move || Move->IsFalling())
	{
		return false;
	}

	return true;
}

ACharacter* URSGameplayAbility_Crouch::GetCharacterFromActorInfo() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}
