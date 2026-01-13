//#include "AbilitySystem/Abilities/Zone/RSGA_Zone_MeteorStrike.h"
//
//#include "WorldObjects/Zones/RSZone_MeteorImpact.h"
//#include "GameFramework/Actor.h"
//
//URSGA_Zone_MeteorStrike::URSGA_Zone_MeteorStrike()
//{
//	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
//	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
//
//	SpawnOffset = FVector(500.f, 0.f, 0.f);
//}
//
//void URSGA_Zone_MeteorStrike::ActivateAbility(
//	const FGameplayAbilitySpecHandle Handle,
//	const FGameplayAbilityActorInfo* ActorInfo,
//	const FGameplayAbilityActivationInfo ActivationInfo,
//	const FGameplayEventData* TriggerEventData)
//{
//	if (!HasAuthority(ActivationInfo))
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//		return;
//	}
//
//	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !MeteorImpactZoneClass)
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
//	const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);
//
//	SpawnZone(MeteorImpactZoneClass, SpawnTM, ActorInfo);
//
//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
//}











//#include "AbilitySystem/Abilities/Zone/RSGA_Zone_MeteorStrike.h"
//
//#include "Character/RSCharacter.h"
//#include "Animation/AnimMontage.h"
//
//URSGA_Zone_MeteorStrike::URSGA_Zone_MeteorStrike()
//{
//	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
//	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
//
//	// 필요하면 에디터에서 SpawnOffset / LifeSpan은 조절
//	// SpawnOffset, LifeSpan은 URSGA_Zone_Base 쪽 멤버를 그대로 사용
//}
//
//void URSGA_Zone_MeteorStrike::TryPlayMeteorStrikeMontage(const FGameplayAbilityActorInfo* ActorInfo) const
//{
//	if (!bPlaySkillMontage || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
//	{
//		return;
//	}
//
//	ARSCharacter* AvatarCharacter = Cast<ARSCharacter>(ActorInfo->AvatarActor.Get());
//	if (!IsValid(AvatarCharacter))
//	{
//		return;
//	}
//
//	// RSCharacter에 이미 존재: GetSkillMeteorMontage()
//	UAnimMontage* Montage = AvatarCharacter->GetSkillMeteorStrikeMontage();
//	if (!IsValid(Montage))
//	{
//		return;
//	}
//
//	// 단발형(ServerOnly + 즉시 EndAbility) 구조라 AbilityTask 대신 단순 재생
//	AvatarCharacter->PlayAnimMontage(Montage, MontagePlayRate);
//}
//
//void URSGA_Zone_MeteorStrike::ActivateAbility(
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
//	// ZoneClass가 비어있으면 실행 불가 (에디터에서 반드시 지정)
//	if (!ZoneClass)
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
//	// (추가) Commit 성공 이후에만 몽타주 재생
//	TryPlayMeteorStrikeMontage(ActorInfo);
//
//	// 기존 베이스 유틸 사용(스폰 로직 건드리지 않음)
//	SpawnSingleZone(ActorInfo);
//
//	// 단발형
//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
//}


#include "AbilitySystem/Abilities/Zone/RSGA_Zone_MeteorStrike.h"

#include "Character/RSCharacter.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "WorldObjects/Zones/RSZone_MeteorImpact.h"

URSGA_Zone_MeteorStrike::URSGA_Zone_MeteorStrike()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 변경 전 기본값 유지 (너가 주석에 남긴 값 그대로)
	SpawnOffset = FVector(500.f, 0.f, 0.f);
}

void URSGA_Zone_MeteorStrike::TryPlayMeteorStrikeMontage(const FGameplayAbilityActorInfo* ActorInfo) const
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

	// RSCharacter에 이미 있는 건 GetSkillMeteorMontage()였음
	// (GetSkillMeteorStrikeMontage()는 없거나 너가 아직 안 만들었을 확률이 큼)
	UAnimMontage* Montage = AvatarCharacter->GetSkillMeteorStrikeMontage();
	if (!IsValid(Montage))
	{
		return;
	}

	AvatarCharacter->PlayAnimMontage(Montage, MontagePlayRate);
}

void URSGA_Zone_MeteorStrike::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
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

	// 핵심: 기존 변수 우선, 없으면 ZoneBase의 ZoneClass로 폴백
	TSubclassOf<AActor> ClassToSpawn = MeteorImpactZoneClass ? MeteorImpactZoneClass : ZoneClass;
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

	// Commit 이후 몽타주 재생 (스폰 로직과 독립)
	TryPlayMeteorStrikeMontage(ActorInfo);

	// 변경 전 스폰 방식 그대로: 위치 계산 후 SpawnZone 직접 호출
	AActor* Avatar = ActorInfo->AvatarActor.Get();

	const FVector SpawnLoc =
		Avatar->GetActorLocation() +
		Avatar->GetActorRotation().RotateVector(SpawnOffset);

	const FTransform SpawnTM(FRotator::ZeroRotator, SpawnLoc);

	SpawnZone(ClassToSpawn, SpawnTM, ActorInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
