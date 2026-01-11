#include "AbilitySystem/Abilities/Install/RSGA_Install_Obstacle.h"

#include "GameFramework/Actor.h"

URSGA_Install_Obstacle::URSGA_Install_Obstacle()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
}

void URSGA_Install_Obstacle::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!HasAuthority(ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !ObstacleClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FVector SpawnLoc =
		Avatar->GetActorLocation() +
		Avatar->GetActorRotation().RotateVector(SpawnOffset);

	const FRotator SpawnRot = Avatar->GetActorRotation();
	const FTransform SpawnTM(SpawnRot, SpawnLoc);

	SpawnDevice(ObstacleClass, SpawnTM, ActorInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
