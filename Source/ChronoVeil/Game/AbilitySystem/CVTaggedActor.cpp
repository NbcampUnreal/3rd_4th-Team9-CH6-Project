#include "Game/AbilitySystem/CVTaggedActor.h"


ACVTaggedActor::ACVTaggedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void ACVTaggedActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void ACVTaggedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

