#include "Game/WorldObjects/Zones/CVZone_Base.h"

#include "Components/ShapeComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACVZone_Base::ACVZone_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	// ZoneShape는 파생 클래스에서 구/박스/캡슐 중 하나를 생성해서
	// RootComponent로 지정하는 패턴을 권장.
	// 여기서는 nullptr 유지.
}

void ACVZone_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 존 주기 처리
		if (TickInterval > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				TickHandle,
				this,
				&ThisClass::ZoneTick,
				TickInterval,
				true
			);
		}

		// 수명 타이머
		if (LifeTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LifeHandle,
				this,
				&ThisClass::HandleLifeTimeExpired,
				LifeTime,
				false
			);
		}
	}
}

void ACVZone_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(TickHandle);
		TM.ClearTimer(LifeHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ACVZone_Base::ZoneTick()
{
	// 서버에서만 실질 로직 수행
	if (!HasAuthority())
	{
		return;
	}

	// 기본 구현: 없음
	// 파생 클래스(연막/버프/디버프/메테오장판/블랙홀)가 오버라이드해서:
	// - ZoneShape->GetOverlappingActors
	// - ASC에 GameplayEffect 부여/제거
	// - DOT/HoT 처리
	// 등을 구현
}

void ACVZone_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}
