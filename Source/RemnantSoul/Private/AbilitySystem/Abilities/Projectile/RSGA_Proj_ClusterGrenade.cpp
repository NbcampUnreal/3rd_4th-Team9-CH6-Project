#include "AbilitySystem/Abilities/Projectile/RSGA_Proj_ClusterGrenade.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PrimitiveComponent.h"

URSGA_Proj_ClusterGrenade::URSGA_Proj_ClusterGrenade()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	/* CV 기준 값. 손/무기 근처에서 나가는 느낌을 위해 X를 줄이고 Z를 올림 */
	SpawnOffset = FVector(70.f, 0.f, 50.f);
}

void URSGA_Proj_ClusterGrenade::ActivateAbility(
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

	const FVector SpawnLoc =
		Avatar->GetActorLocation()
		+ Avatar->GetActorForwardVector() * SpawnOffset.X
		+ Avatar->GetActorRightVector() * SpawnOffset.Y
		+ Avatar->GetActorUpVector() * SpawnOffset.Z;

	const FRotator SpawnRot = Avatar->GetActorRotation();
	const FTransform SpawnTM(SpawnRot, SpawnLoc);

	AActor* GrenadeActor = SpawnProjectile(ProjectileClass, SpawnTM, ActorInfo);
	if (!GrenadeActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	/* 던질 방향: Forward + Up * tan(pitch) */
	const FVector Forward = Avatar->GetActorForwardVector();
	const FVector Up = Avatar->GetActorUpVector();

	const float PitchRad = FMath::DegreesToRadians(ThrowPitchAngle);
	const FVector ThrowDir = (Forward + Up * FMath::Tan(PitchRad)).GetSafeNormal();

	bool bLaunched = false;

	/* 1) Root Primitive + Physics Impulse */
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GrenadeActor->GetRootComponent()))
	{
		if (RootPrim->IsSimulatingPhysics())
		{
			RootPrim->AddImpulse(ThrowDir * ThrowStrength, NAME_None, true);
			bLaunched = true;
		}
	}

	/* 2) Fallback: ProjectileMovementComponent Velocity */
	if (!bLaunched)
	{
		if (UProjectileMovementComponent* PMC = GrenadeActor->FindComponentByClass<UProjectileMovementComponent>())
		{
			PMC->Velocity = ThrowDir * FallbackInitialSpeed;
			bLaunched = true;
		}
	}

	/* 단발형: 스폰/발사하면 종료 */
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
