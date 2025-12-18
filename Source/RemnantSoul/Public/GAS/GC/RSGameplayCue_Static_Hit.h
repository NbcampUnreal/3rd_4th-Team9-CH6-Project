#pragma once

#include "GameplayCueNotify_Static.h"
#include "RSGameplayCue_Static_Hit.generated.h"

class UParticleSystem;

UCLASS()
class REMNANTSOUL_API URSGameplayCue_Static_Hit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	URSGameplayCue_Static_Hit();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UParticleSystem> ParticleSystem;

};