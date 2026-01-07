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
UCLASS()
class REMNANTSOUL_API ARSLeverActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSLeverActor();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	// ===== SFX =====
	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundBase> SFX_PullStart = nullptr;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundBase> SFX_PullEnd = nullptr; // 딸깍(선택)

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	TObjectPtr<USoundAttenuation> SFX_Attenuation = nullptr;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	float SFX_Volume = 1.0f;

	UPROPERTY(EditAnywhere, Category="Lever|SFX")
	float SFX_Pitch = 1.0f;

	void PlaySFX(USoundBase* Sound) const;
	FVector GetSFXLocation() const;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	bool PassesTagGate(AActor* Interactor) const;

	void StartLeverAnim();
	void UpdateLeverAnim(float DeltaSeconds);
	void TriggerLinkedTargets(AActor* Interactor);
	void OnFocusBegin_Implementation(AActor* Interactor);
	void OnFocusEnd_Implementation(AActor* Interactor);

private:
	UPROPERTY(VisibleAnywhere, Category="Lever")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category="Lever")
	TObjectPtr<USceneComponent> Pivot;

	UPROPERTY(VisibleAnywhere, Category="Lever")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, Category="Lever")
	TObjectPtr<UStaticMeshComponent> HandleMesh;

	// ===== One-shot =====
	UPROPERTY(VisibleInstanceOnly, Category="Lever|OneShot")
	bool bHasFired = false;

	// ===== Tag Gate =====
	UPROPERTY(EditAnywhere, Category="Lever|TagGate")
	FGameplayTagContainer InteractorRequiredTags;

	UPROPERTY(EditAnywhere, Category="Lever|TagGate")
	FGameplayTagContainer InteractorBlockedTags;

	// ===== Linked =====
	UPROPERTY(EditAnywhere, Category="Lever|Linked")
	TArray<TObjectPtr<AActor>> LinkedTargets;
	
	UPROPERTY(EditAnywhere, Category="Lever|Linked")
	bool bTriggerOnAnimFinished = true;

	// ===== Anim =====
	UPROPERTY(EditAnywhere, Category="Lever|Anim")
	FVector LocalAxis = FVector(1.0f, 0.0f, 0.0f); 

	UPROPERTY(EditAnywhere, Category="Lever|Anim")
	float AngleDegrees = -45.0f;

	UPROPERTY(EditAnywhere, Category="Lever|Anim", meta=(ClampMin="0.01"))
	float PullTime = 0.2f;


	
	FQuat StartQuat;
	FQuat TargetQuat;
	
	bool bAnimating = false;
	float AnimElapsed = 0.0f;
	FRotator AnimStartRot;
	FRotator AnimTargetRot;

	TWeakObjectPtr<AActor> PendingInteractor;
};
