#include "WorldObjects/Devices/RSDevice_Obstacle.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ARSDevice_Obstacle::ARSDevice_Obstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	// ReplicateMovement는 ARSDevice_Base에서 이미 켜져있음.
	// (필요하면 여기서 다시 켜도 무방)
	// bReplicates = true;
	// SetReplicateMovement(true);
}

void ARSDevice_Obstacle::BeginPlay()
{
	Super::BeginPlay();

	// 서버가 위치를 움직이고 ReplicateMovement로 클라에 전파하는 패턴 권장
	if (HasAuthority())
	{
		BeginRise();
	}
	else
	{
		// 클라는 ReplicateMovement 따라가면 됨. Tick은 굳이 필요 없음.
		// 다만, 프로젝트 정책에 따라 클라에서도 보간을 하고 싶으면 남겨도 됨.
		SetActorTickEnabled(false);
	}
}

void ARSDevice_Obstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || !bRising)
	{
		return;
	}

	RiseElapsed += DeltaTime;

	const float LinearAlpha = (RiseDuration > 0.f) ? FMath::Clamp(RiseElapsed / RiseDuration, 0.f, 1.f) : 1.f;
	const float Alpha = EvalAlpha(LinearAlpha);

	const FVector NewLoc = FMath::Lerp(RiseStartLoc, RiseTargetLoc, Alpha);
	SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

	if (LinearAlpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		FinishRise();
	}
}

void ARSDevice_Obstacle::BeginRise()
{
	bRising = true;
	RiseElapsed = 0.f;

	RiseTargetLoc = GetActorLocation();
	RiseStartLoc = RiseTargetLoc - FVector(0.f, 0.f, RiseStartDepth);

	// 시작 위치로 순간 이동
	SetActorLocation(RiseStartLoc, false, nullptr, ETeleportType::TeleportPhysics);

	if (bDisableCollisionWhileRising)
	{
		if (Collision)
		{
			Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (Mesh)
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ARSDevice_Obstacle::FinishRise()
{
	bRising = false;

	// 목표 위치로 정렬
	SetActorLocation(RiseTargetLoc, false, nullptr, ETeleportType::TeleportPhysics);

	if (bDisableCollisionWhileRising)
	{
		if (Collision)
		{
			// 기존 베이스가 OverlapAllDynamic을 쓰고 있으니, “방벽”이라면 Block 계열로 바꾸는 걸 추천
			Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		if (Mesh)
		{
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

float ARSDevice_Obstacle::EvalAlpha(float LinearAlpha) const
{
	if (!bUseEaseOut)
	{
		return LinearAlpha;
	}

	// EaseOutQuad 느낌 (0~1)
	// Alpha = 1 - (1 - t)^2
	const float OneMinus = 1.f - LinearAlpha;
	return 1.f - (OneMinus * OneMinus);
}
