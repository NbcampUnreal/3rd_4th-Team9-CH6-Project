#include "Game/AbilitySystem/Abilities/Install/CVGA_Install_AffectionDoll.h"

#include "Game/WorldObjects/Devices/CVDevice_AffectionDoll.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

UCVGA_Install_AffectionDoll::UCVGA_Install_AffectionDoll()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
}

void UCVGA_Install_AffectionDoll::ActivateAbility(
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

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !AffectionDollClass)
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

	// 캐릭터 앞쪽 위치 계산
	const FVector SpawnLoc =
		Avatar->GetActorLocation() +
		Avatar->GetActorRotation().RotateVector(SpawnOffset);

	const FRotator SpawnRot = Avatar->GetActorRotation();
	const FTransform SpawnTM(SpawnRot, SpawnLoc);

	ACVDevice_Base* SpawnedDevice = SpawnDevice(
		AffectionDollClass,
		SpawnTM,
		ActorInfo
	);

	// 필요하면 캐스팅해서 추가 초기화 가능
	// if (ACVDevice_AffectionDoll* Doll = Cast<ACVDevice_AffectionDoll>(SpawnedDevice))
	// {
	//     ...
	// }

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
