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

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	Character->Jump();

	SetWaitingOnAvatar();
	// 어빌리티 태스크의 종료를 미루고자 한다면 이 함수를 호출해서 Waiting 상태로 진입.
	// 만약 Tick을 활성화 하고 싶다면 bTickingTask 값을 true로 설정.
}

void URSAbilityTask_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);

	Super::OnDestroy(AbilityEnded);
}

void URSAbilityTask_JumpAndWaitForLanding::OnLanded(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
		// 어빌리티 쪽으로 종료를 알림.
	}
}
