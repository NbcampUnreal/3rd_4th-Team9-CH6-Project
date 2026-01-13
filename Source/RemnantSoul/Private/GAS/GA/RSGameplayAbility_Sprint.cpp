#include "GAS/GA/RSGameplayAbility_Sprint.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Sprint)

URSGameplayAbility_Sprint::URSGameplayAbility_Sprint()
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// 프로젝트 태그 네이밍에 맞춰 RSGameplayTags 이미 만들어둔 걸로 연결
	// 예: Ability.Sprint / Status.Sprint 같은 형태
	AbilityTags.AddTag(Tags.Ability_Sprint);
	ActivationOwnedTags.AddTag(Tags.Status_Sprint);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 입력 홀드형으로 쓰기 좋게: Activate 후 유지, Released 시 End
	// (프레임워크는 TryActivateAbility로 켜주고, InputReleased로 끄는 구조)
}

void URSGameplayAbility_Sprint::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CanSprintNow())
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UCharacterMovementComponent* MoveComp = GetMovementFromActorInfo();
	if (!MoveComp)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// 이미 스프린트 중이면 중복 적용 방지
	// (같은 Ability를 다시 Activate하는 케이스가 들어오면 안정적으로 종료)
	if (CachedWalkSpeed > 0.f)
	{
		// 이미 캐시가 있으면 스프린트 적용 중으로 판단
		return;
	}

	CachedWalkSpeed = MoveComp->MaxWalkSpeed;
	ApplySprintSpeed(MoveComp);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_SprintDrain);
		World->GetTimerManager().SetTimer(
			TimerHandle_SprintDrain,
			this,
			&ThisClass::TickSprintDrain,
			SprintDrainInterval,
			true);
	}
}

void URSGameplayAbility_Sprint::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 핵심: RSHeroComponent가 Released를 ASC로 전달하면,
	// AbilitySpecInputReleased -> 이 함수가 호출되는 흐름이 된다.
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void URSGameplayAbility_Sprint::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UCharacterMovementComponent* MoveComp = GetMovementFromActorInfo())
	{
		RestoreWalkSpeed(MoveComp);
	}

	CachedWalkSpeed = 0.f;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_SprintDrain);
	}
}

bool URSGameplayAbility_Sprint::CanSprintNow() const
{
	ACharacter* Char = GetCharacterFromActorInfo();
	if (!Char)
	{
		return false;
	}

	UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement();
	if (!MoveComp || MoveComp->IsFalling())
	{
		return false;
	}

	// 선택: 이동 입력이 없으면 스프린트 불가로 하고 싶다면 활성화
	// if (MoveComp->GetCurrentAcceleration().IsNearlyZero())
	// {
	//     return false;
	// }

	return true;
}

ACharacter* URSGameplayAbility_Sprint::GetCharacterFromActorInfo() const
{
	return Cast<ACharacter>(GetAvatarActorFromActorInfo());
}

UCharacterMovementComponent* URSGameplayAbility_Sprint::GetMovementFromActorInfo() const
{
	if (ACharacter* Char = GetCharacterFromActorInfo())
	{
		return Char->GetCharacterMovement();
	}
	return nullptr;
}

void URSGameplayAbility_Sprint::ApplySprintSpeed(UCharacterMovementComponent* MoveComp)
{
	if (!MoveComp)
	{
		return;
	}

	if (bUseFixedSprintSpeed)
	{
		MoveComp->MaxWalkSpeed = SprintWalkSpeed;
	}
	else
	{
		MoveComp->MaxWalkSpeed = MoveComp->MaxWalkSpeed * SprintSpeedMultiplier;
	}
}

void URSGameplayAbility_Sprint::RestoreWalkSpeed(UCharacterMovementComponent* MoveComp)
{
	if (!MoveComp)
	{
		return;
	}

	// 캐시가 유효할 때만 원복
	if (CachedWalkSpeed > 0.f)
	{
		MoveComp->MaxWalkSpeed = CachedWalkSpeed;
	}
}

void URSGameplayAbility_Sprint::TickSprintDrain()
{
	if (!SprintStaminaTickEffectClass)
	{
		return;
	}

	const UGameplayEffect* GE = SprintStaminaTickEffectClass->GetDefaultObject<UGameplayEffect>();
	ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GE, 1);
}
