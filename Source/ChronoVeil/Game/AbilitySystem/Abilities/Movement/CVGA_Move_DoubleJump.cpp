#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_DoubleJump.h"
#include "Game/Character/CVCharacter.h"

void UCVGA_Move_DoubleJump::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 코스트/쿨다운 커밋
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACVCharacter* Character = nullptr;

	// CVGameplayAbility 쪽에 헬퍼가 있으면 그거 써도 되고,
	// 없으면 이렇게 직접 AvatarActor에서 캐스팅
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		Character = Cast<ACVCharacter>(ActorInfo->AvatarActor.Get());
	}

	if (Character)
	{
		// 위로 강하게 튀어오르는 더블 점프
		Character->LaunchCharacter(FVector(0.f, 0.f, JumpStrength), false, true);
	}

	// 즉시 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}