
#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Base.h"

void UCVGA_Move_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 공통: 코스트/쿨다운 커밋
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 기본 구현은 바로 종료 (실제 이동은 파생 클래스에서 수행)
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}