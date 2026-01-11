#include "AbilitySystem/Abilities/Projectile/RSGA_Proj_Armageddon.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"

URSGA_Proj_Armageddon::URSGA_Proj_Armageddon()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpawnOffset = FVector(200.f, 0.f, 50.f);
	LifeSpan = 0.f;
}

bool URSGA_Proj_Armageddon::ExtractTargetLocation(
	const FGameplayEventData* TriggerEventData,
	FVector& OutTargetLocation,
	TWeakObjectPtr<AActor>& OutTargetActor
) const
{
	OutTargetActor = nullptr;
	OutTargetLocation = FVector::ZeroVector;

	if (!TriggerEventData)
	{
		return false;
	}

	/* 1) Target Actor 우선 */
	//if (TriggerEventData->Target)
	//{
	//	OutTargetActor = TriggerEventData->Target;
	//	OutTargetLocation = TriggerEventData->Target->GetActorLocation();
	//	return true;
	//}

	/* 2) TargetData의 HitResult */
	if (TriggerEventData->TargetData.Num() > 0)
	{
		const FGameplayAbilityTargetData* TD0 = TriggerEventData->TargetData.Get(0);
		if (TD0)
		{
			/* HitResult 기반 */
			if (TD0->HasHitResult())
			{
				const FHitResult* HR = TD0->GetHitResult();
				if (HR)
				{
					OutTargetLocation = HR->ImpactPoint;
					if (HR->GetActor())
					{
						OutTargetActor = HR->GetActor();
					}
					return true;
				}
			}

			/* Actor 배열 기반 */
			const TArray<TWeakObjectPtr<AActor>> Actors = TD0->GetActors();
			for (const TWeakObjectPtr<AActor>& A : Actors)
			{
				if (A.IsValid())
				{
					OutTargetActor = A;
					OutTargetLocation = A->GetActorLocation();
					return true;
				}
			}
		}
	}

	return false;
}

void URSGA_Proj_Armageddon::ActivateAbility(
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

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !ProjectileClass)
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

	/* 스폰 위치 계산 (Forward/Right/Up) */
	const FVector SpawnLoc =
		Avatar->GetActorLocation()
		+ Avatar->GetActorForwardVector() * SpawnOffset.X
		+ Avatar->GetActorRightVector() * SpawnOffset.Y
		+ Avatar->GetActorUpVector() * SpawnOffset.Z;

	/* 목표 위치(있으면) */
	FVector TargetLoc = FVector::ZeroVector;
	TWeakObjectPtr<AActor> TargetActor;

	const bool bHasTarget =
		(bUseTriggerTarget && ExtractTargetLocation(TriggerEventData, TargetLoc, TargetActor));

	/* 회전: 목표가 있으면 목표를 향하도록 */
	const FRotator SpawnRot = bHasTarget
		? (TargetLoc - SpawnLoc).Rotation()
		: Avatar->GetActorRotation();

	const FTransform SpawnTM(SpawnRot, SpawnLoc);

	AActor* ProjActor = SpawnProjectile(ProjectileClass, SpawnTM, ActorInfo);
	if (!ProjActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	/* 발사 방향 */
	const FVector LaunchDir = SpawnRot.Vector();

	/* ProjectileMovementComponent가 있으면 Velocity/Homing 설정 */
	if (UProjectileMovementComponent* PMC = ProjActor->FindComponentByClass<UProjectileMovementComponent>())
	{
		PMC->Velocity = LaunchDir * InitialSpeed;

		if (bUseHomingIfPossible && bHasTarget && TargetActor.IsValid())
		{
			if (USceneComponent* TargetComp = TargetActor->GetRootComponent())
			{
				PMC->bIsHomingProjectile = true;
				PMC->HomingTargetComponent = TargetComp;
				PMC->HomingAccelerationMagnitude = HomingAcceleration;
			}
		}
	}
	else
	{
		/* 없으면 Root Primitive에 Impulse */
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(ProjActor->GetRootComponent()))
		{
			if (Prim->IsSimulatingPhysics())
			{
				Prim->AddImpulse(LaunchDir * ImpulseStrength, NAME_None, true);
			}
		}
	}

	/* 단발형: 스폰하면 종료 */
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
