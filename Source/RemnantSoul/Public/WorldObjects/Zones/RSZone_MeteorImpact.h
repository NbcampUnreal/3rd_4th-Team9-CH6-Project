#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/Zones/RSZone_Base.h"
#include "RSZone_MeteorImpact.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class REMNANTSOUL_API ARSZone_MeteorImpact : public ARSZone_Base
{
	GENERATED_BODY()

public:
	ARSZone_MeteorImpact();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> MeteorVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> FallingVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float ImpactDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float Damage = 50.f;

	FTimerHandle ImpactTimer;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnImpact();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayImpactVFX();
};
