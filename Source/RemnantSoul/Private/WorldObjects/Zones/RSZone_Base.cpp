#include "WorldObjects/Zones/RSZone_Base.h"

#include "Components/ShapeComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARSZone_Base::ARSZone_Base()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARSZone_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
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

void ARSZone_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(TickHandle);
		TM.ClearTimer(LifeHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ARSZone_Base::ZoneTick()
{
	if (!HasAuthority())
	{
		return;
	}
}

void ARSZone_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	Destroy();
}
