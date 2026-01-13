
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameplayTagContainer.h"
#include "RSLeverActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USoundBase;
class USoundAttenuation;
class ARSMetalDoor;

UCLASS()
class REMNANTSOUL_API ARSLeverActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSLeverActor();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// Tag gate
	bool PassesTagGate(AActor* Interactor) const;

	void StartLeverAnim();
	void UpdateLeverAnim(float DeltaSeconds);

	// Linked targets trigger
	void TriggerLinkedTargets(AActor* Interactor);

	// SFX
	void PlaySFX(USoundBase* Sound) const;
	void OnFocusBegin_Implementation(AActor* Interactor);
	void OnFocusEnd_Implementation(AActor* Interactor);
	FVector GetSFXLocation() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Lever|Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Lever|Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Lever|Components")
	TObjectPtr<USceneComponent> Pivot = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Lever|Components")
	TObjectPtr<UStaticMeshComponent> HandleMesh = nullptr;

	// ===== Settings =====
	UPROPERTY(EditAnywhere, Category="Lever|Settings")
	bool bHasFired = false;

	UPROPERTY(EditAnywhere, Category="Lever|Settings")
	bool bTriggerOnAnimFinished = true;

	UPROPERTY(EditAnywhere, Category="Lever|Settings")
	FVector LocalAxis = FVector(1,0,0);

	UPROPERTY(EditAnywhere, Category="Lever|Settings")
	float AngleDegrees = 45.0f;

	UPROPERTY(EditAnywhere, Category="Lever|Settings", meta=(ClampMin="0.01"))
	float PullTime = 0.25f;

	UPROPERTY(EditAnywhere, Category="Lever|Targets")
	TArray<TObjectPtr<AActor>> LinkedTargets;

	// ===== Tag Gate =====
	UPROPERTY(EditAnywhere, Category="Lever|TagGate")
	FGameplayTagContainer InteractorRequiredTags;

	UPROPERTY(EditAnywhere, Category="Lever|TagGate")
	FGameplayTagContainer InteractorBlockedTags;

	// ===== SFX =====
	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundBase> SFX_PullStart = nullptr;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundBase> SFX_PullEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundAttenuation> SFX_Attenuation = nullptr;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	float SFX_Volume = 1.0f;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	float SFX_Pitch = 1.0f;

	// ===== Runtime =====
	bool bAnimating = false;
	float AnimElapsed = 0.0f;

	FQuat StartQuat;
	FQuat TargetQuat;

	TWeakObjectPtr<AActor> PendingInteractor;
};
