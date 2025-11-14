
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CVTaggedActor.generated.h"

UCLASS()
class CHRONOVEIL_API ACVTaggedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACVTaggedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
