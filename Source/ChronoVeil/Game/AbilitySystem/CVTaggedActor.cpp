
#include "Game/AbilitySystem/CVTaggedActor.h"

// Sets default values
ACVTaggedActor::ACVTaggedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACVTaggedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACVTaggedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

