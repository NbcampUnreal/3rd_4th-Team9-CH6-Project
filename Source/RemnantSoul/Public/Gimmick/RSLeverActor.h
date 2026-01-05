#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameplayTagContainer.h"
#include "RSLeverActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

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
	bool PassesTagGate(AActor* Interactor) const;

	void StartLeverAnim();
	void UpdateLeverAnim(float DeltaSeconds);
	void TriggerLinkedTargets(AActor* Interactor);

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

	// ===== Tag Gate (선택) =====
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
	FVector LocalAxis = FVector(1.0f, 0.0f, 0.0f); // Pivot 로컬 X축 (원하는 축으로 바꾸면 됨)

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
