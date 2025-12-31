// RSAbilityTask_JumpAndWaitForLanding.cpp


#include "GAS/AT/RSAbilityTask_JumpAndWaitForLanding.h"
#include "GameFramework/Character.h"

URSAbilityTask_JumpAndWaitForLanding::URSAbilityTask_JumpAndWaitForLanding()
{
}

URSAbilityTask_JumpAndWaitForLanding* URSAbilityTask_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
	URSAbilityTask_JumpAndWaitForLanding* NewTask = NewAbilityTask<URSAbilityTask_JumpAndWaitForLanding>(OwningAbility);
	return NewTask;
}

void URSAbilityTask_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (!IsValid(Character))
	{
		EndTask();
		return;
	}

	CachedCharacter = Character;

	BindLanded();
	Character->Jump();

	SetWaitingOnAvatar();
}

void URSAbilityTask_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	UnbindLanded();
	Super::OnDestroy(AbilityEnded);
}

void URSAbilityTask_JumpAndWaitForLanding::OnLanded(const FHitResult& Hit)
{
	UnbindLanded();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
	}

	EndTask();
}

void URSAbilityTask_JumpAndWaitForLanding::ExternalCancel()
{
	UnbindLanded();
	Super::ExternalCancel();
	EndTask();
}

void URSAbilityTask_JumpAndWaitForLanding::BindLanded()
{
	if (bBoundToLanded)
	{
		return;
	}

	ACharacter* Character = CachedCharacter.Get();
	if (!IsValid(Character))
	{
		return;
	}

	Character->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	bBoundToLanded = true;
}

void URSAbilityTask_JumpAndWaitForLanding::UnbindLanded()
{
	if (!bBoundToLanded)
	{
		return;
	}

	ACharacter* Character = CachedCharacter.Get();
	if (IsValid(Character))
	{
		Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);
	}

	bBoundToLanded = false;
	CachedCharacter = nullptr;
}
