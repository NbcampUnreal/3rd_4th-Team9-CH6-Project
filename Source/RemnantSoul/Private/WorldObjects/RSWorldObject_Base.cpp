#include "WorldObjects/RSWorldObject_Base.h"

ARSWorldObject_Base::ARSWorldObject_Base()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARSWorldObject_Base::BeginPlay()
{
	Super::BeginPlay();
}

void ARSWorldObject_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARSWorldObject_Base::SetOwnerFromASC(AActor* InOwnerFromASC)
{
	OwnerFromASC = InOwnerFromASC;

	/* Owner/Instigator도 같이 세팅해두면 Damage/UGameplayStatics 경로에서 유리함 */
	if (InOwnerFromASC)
	{
		SetOwner(InOwnerFromASC);
	}
}

AActor* ARSWorldObject_Base::GetOwnerFromASC() const
{
	return OwnerFromASC.Get();
}
