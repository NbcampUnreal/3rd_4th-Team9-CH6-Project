#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/RSWorldObject_Base.h"
#include "RSZone_Base.generated.h"

class UShapeComponent;

UCLASS()
class REMNANTSOUL_API ARSZone_Base : public ARSWorldObject_Base
{
	GENERATED_BODY()

public:
	ARSZone_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Zone")
	TObjectPtr<UShapeComponent> ZoneShape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Zone")
	float LifeTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Zone")
	float TickInterval = 0.2f;

	FTimerHandle LifeHandle;
	FTimerHandle TickHandle;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void ZoneTick();

	UFUNCTION()
	virtual void HandleLifeTimeExpired();
};
