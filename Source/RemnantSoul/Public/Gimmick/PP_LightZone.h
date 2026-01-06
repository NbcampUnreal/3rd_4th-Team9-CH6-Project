#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PP_LightZone.generated.h"

class UBoxComponent;
class UPostProcessComponent;
class ADirectionalLight;

UCLASS()
class REMNANTSOUL_API APP_LightZone : public AActor
{
	GENERATED_BODY()

public:
	APP_LightZone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UPostProcessComponent* PostProcess;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Light")
	ADirectionalLight* TargetDirectionalLight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
	FName DirectionalLightTag = TEXT("AutoDirLight");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
	float LightIntensity_On = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
	float LightIntensity_Off = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
	float LightFadeSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PostProcess")
	float PPBlendSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
	bool bToggleVisibility = true;

	bool bPlayerInside = false;

	float CurrentPPBlend = 0.0f;
	float CurrentLightIntensity = 0.0f;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	bool IsPlayerActor(const AActor* Actor) const;
	void ResolveDirectionalLight();
	void ApplyLight(float DeltaSeconds);
	void ApplyPostProcess(float DeltaSeconds);
};
