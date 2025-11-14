#include "Game/WorldObjects/CVWorldObject_Base.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

ACVWorldObject_Base::ACVWorldObject_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACVWorldObject_Base::SetOwnerFromASC(AActor* InAvatar)
{
	if (!InAvatar)
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(InAvatar))
	{
		OwningPawn = Pawn;
		OwningController = Pawn->GetController();
	}
	else if (AController* Controller = Cast<AController>(InAvatar))
	{
		OwningController = Controller;
		OwningPawn = Controller->GetPawn();
	}
}