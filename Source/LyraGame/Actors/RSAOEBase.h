#pragma once

#include "RSAOEBase.generated.h"

class ULegacyCameraShake;
class USphereComponent;
class UArrowComponent;

UCLASS(BlueprintType, Abstract)
class ARSAOEBase : public AActor
{
	GENERATED_BODY()
	
public:
	ARSAOEBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

private:
	void StartAOE();
	void TickAOE();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="RS|AOE")
	TSubclassOf<AActor> AOEElementClass;

	UPROPERTY(EditDefaultsOnly, Category="RS|AOE")
	float StartDelay = 2.f;
	
	UPROPERTY(EditDefaultsOnly, Category="RS|AOE")
	float AttackTotalTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="RS|AOE")
	int32 TargetAttackCount = 5;
	
	UPROPERTY(EditDefaultsOnly, Category="RS|AOE")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;
	
private:
	int32 CurrentAttackCount = 0;
	float AttackIntervalTime = 0.f;
	FTimerHandle AOETimerHandle;
};
