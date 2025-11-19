#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CVTaggedActor.generated.h"

UCLASS()
class CHRONOVEIL_API ACVTaggedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACVTaggedActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
