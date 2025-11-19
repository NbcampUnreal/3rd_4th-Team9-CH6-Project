#include "Game/WorldObjects/Devices/CVDevice_AffectionDoll.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ACVDevice_AffectionDoll::ACVDevice_AffectionDoll()
{
	PrimaryActorTick.bCanEverTick = true;

	if (Collision)
	{
		Collision->InitSphereRadius(PullRadius);
		Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		Collision->SetGenerateOverlapEvents(true);

		Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
		Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	LifeTime = 2.0f;

	// 네트워크
	bReplicates = true;
	SetReplicateMovement(true);
}

void ACVDevice_AffectionDoll::BeginPlay()
{
	Super::BeginPlay();

	SpawnTime = GetWorld()->GetTimeSeconds();
	bIsGrowing = true;

	SetActorScale3D(StartScale);

	if (Collision)
	{
		Collision->SetSphereRadius(PullRadius);
	}
}

void ACVDevice_AffectionDoll::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeSinceSpawn = GetWorld()->GetTimeSeconds() - SpawnTime;

	if (bIsGrowing && GrowDuration > 0.f)
	{
		const float Alpha = FMath::Clamp(TimeSinceSpawn / GrowDuration, 0.f, 1.f);
		const FVector NewScale = FMath::Lerp(StartScale, TargetScale, Alpha);
		SetActorScale3D(NewScale);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			bIsGrowing = false;
		}
	}

	if (TimeSinceSpawn <= PullDuration)
	{
		ApplyPull(DeltaTime);
	}
}

void ACVDevice_AffectionDoll::ApplyPull(float DeltaTime)
{
	if (!HasAuthority() || !Collision)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	const FVector Center = GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		ACharacter* Character = Cast<ACharacter>(Actor);
		if (!Character)
		{
			continue;
		}

		// 자기 소유자(설치자)를 끌어당길지 여부는 정책에 따라 선택
		// if (Character == GetOwningPawn()) 
		// {
		//		Continue;
		// }

		const FVector CharLoc = Character->GetActorLocation();
		FVector ToCenter = Center - CharLoc;
		ToCenter.Z = 0.f; // 수평 방향만 끌어당기고 싶으면 Z 제거함.

		const float Distance = ToCenter.Size();
		if (Distance <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		FVector Dir = ToCenter / Distance;

		// DeltaTime 반영해서 frame-rate(시간비율) 독립적인 힘을 쓰는 느낌을 줌.
		const float Strength = PullStrength * DeltaTime;
		FVector LaunchVel = Dir * Strength;

		// 기존 Z 속도는 유지, XY 만 덮어쓰기
		FVector CurrentVel = Character->GetVelocity();
		LaunchVel.Z = CurrentVel.Z;

		// XY Override = true, Z Override = false : 말그대로 xy축 기준으로 끌더 당기는 힘을 적용하고 싶은거임.
		Character->LaunchCharacter(LaunchVel, true, false);
	}
}
