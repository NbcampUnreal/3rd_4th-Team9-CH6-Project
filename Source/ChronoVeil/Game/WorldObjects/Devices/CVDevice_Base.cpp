#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACVDevice_Base::ACVDevice_Base()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACVDevice_Base::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// 수명 타이머
		if (LifeTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LifeTimerHandle,
				this,
				&ThisClass::HandleLifeTimeExpired,
				LifeTime,
				false
			);
		}

		// 필요하면 Owner 죽을 때 Destroy 연동은 나중에 ASC / Health 쪽에서 이벤트 받아서 처리
	}
}

void ACVDevice_Base::HandleLifeTimeExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	// 공통: 수명 끝나면 파괴
	Destroy();
}

void ACVDevice_Base::OnDamaged(float Damage, AActor* DamageCauser)
{
	// 여기서는 아무것도 안 함.
	// 파괴 가능한 장치에서 오버라이드해서 체력 깎고 0 이하면 Destroy() 같은 패턴으로 사용.
}

void ACVDevice_Base::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}


	if (!OtherActor || OtherActor == this)
	{
		return;
	}
}