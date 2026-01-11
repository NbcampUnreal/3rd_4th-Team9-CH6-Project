//#include "AbilitySystem/Abilities/Install/RSGA_Install_Obstacle.h"
//
//#include "GameFramework/Actor.h"
//
//URSGA_Install_Obstacle::URSGA_Install_Obstacle()
//{
//	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
//	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
//
//	SpawnOffset = FVector(200.f, 0.f, 0.f);
//}
//
//void URSGA_Install_Obstacle::ActivateAbility(
//	const FGameplayAbilitySpecHandle Handle,
//	const FGameplayAbilityActorInfo* ActorInfo,
//	const FGameplayAbilityActivationInfo ActivationInfo,
//	const FGameplayEventData* TriggerEventData
//)
//{
//	if (!HasAuthority(ActivationInfo))
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//		return;
//	}
//
//	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//		return;
//	}
//
//	// 핵심: ObstacleClass 우선, 없으면 Install_Base의 DeviceClass로 폴백
//	TSubclassOf<AActor> ClassToSpawn = ObstacleClass ? ObstacleClass : DeviceClass;
//	if (!ClassToSpawn)
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//		return;
//	}
//
//	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
//		return;
//	}
//
//	AActor* Avatar = ActorInfo->AvatarActor.Get();
//
//	const FVector SpawnLoc =
//		Avatar->GetActorLocation() +
//		Avatar->GetActorRotation().RotateVector(SpawnOffset);
//
//	const FTransform SpawnTM(Avatar->GetActorRotation(), SpawnLoc);
//
//	SpawnDevice(ClassToSpawn, SpawnTM, ActorInfo);
//
//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
//}


#include "AbilitySystem/Abilities/Install/RSGA_Install_Obstacle.h"

#include "GameFramework/Actor.h"
#include "Character/RSCharacter.h"
#include "Animation/AnimMontage.h"

URSGA_Install_Obstacle::URSGA_Install_Obstacle()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 0.f);
}

void URSGA_Install_Obstacle::TryPlayObstacleMontage(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!bPlaySkillMontage || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	ARSCharacter* AvatarCharacter = Cast<ARSCharacter>(ActorInfo->AvatarActor.Get());
	if (!IsValid(AvatarCharacter))
	{
		return;
	}

	UAnimMontage* Montage = AvatarCharacter->GetSkillInstallObstacleMontage();
	if (!IsValid(Montage))
	{
		return;
	}

	// ServerOnly Ability라도 서버에서 PlayAnimMontage를 호출하면 (캐릭터 기준)
	// 일반적으로 네트워크에 자연스럽게 반영되는 편.
	AvatarCharacter->PlayAnimMontage(Montage, MontagePlayRate);
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

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 핵심: ObstacleClass 우선, 없으면 Install_Base의 DeviceClass로 폴백
	TSubclassOf<AActor> ClassToSpawn = ObstacleClass ? ObstacleClass : DeviceClass;
	if (!ClassToSpawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// (추가) Commit 이후 몽타주 재생만 끼워 넣음. 기존 로직/흐름은 유지.
	TryPlayObstacleMontage(ActorInfo);

	AActor* Avatar = ActorInfo->AvatarActor.Get();

	const FVector SpawnLoc =
		Avatar->GetActorLocation() +
		Avatar->GetActorRotation().RotateVector(SpawnOffset);

	const FTransform SpawnTM(Avatar->GetActorRotation(), SpawnLoc);

	SpawnDevice(ClassToSpawn, SpawnTM, ActorInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
