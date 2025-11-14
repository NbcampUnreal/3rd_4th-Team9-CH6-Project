#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Dash.h"
#include "Game/Character/CVCharacter.h"

void UCVGA_Move_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACVCharacter* Character = nullptr;
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		Character = Cast<ACVCharacter>(ActorInfo->AvatarActor.Get());
	}

	if (Character)
	{
		// 컨트롤러 Yaw 기준으로 평면 방향 계산
		const FRotator ControlRot(0.f, Character->GetControlRotation().Yaw, 0.f);
		FVector DashDir = ControlRot.Vector();
		DashDir.Z = 0.f;
		DashDir.Normalize();

		// Launch로 순간 가속(지금은 아주 단순)
		Character->LaunchCharacter(DashDir * DashStrength, true, true);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}