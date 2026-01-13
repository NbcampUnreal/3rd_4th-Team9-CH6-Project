#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"

#include "RSRuneNode.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

/**
 * Simple interactable rune node.
 * - Interact: cycles a local state and rotates the visual mesh.
 * - Focus begin/end: optional custom-depth highlight.
 *
 * Note: This is intentionally decoupled from the Puzzle Manager system.
 */
UCLASS()
class REMNANTSOUL_API ARSRuneNode : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSRuneNode();

	// IInteractable
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void OnFocusBegin_Implementation(AActor* Interactor) override;
	virtual void OnFocusEnd_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Visual = nullptr;

	/** Blocks visibility trace so your GA_Interact line trace can hit this actor reliably. */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> Trigger = nullptr;

private:
	/** Number of rune states to cycle through (0..MaxState-1). */
	UPROPERTY(EditAnywhere, Category = "Rune", meta = (ClampMin = "1"))
	int32 MaxState = 6;

	/** Current state index. */
	UPROPERTY(VisibleInstanceOnly, Category = "Rune")
	int32 LocalState = 0;

	/** Base rotation before applying state rotation. */
	UPROPERTY(EditAnywhere, Category = "Rune")
	FRotator BaseRotation = FRotator::ZeroRotator;

	/** Rotation axis for the state step (default: Up). */
	UPROPERTY(EditAnywhere, Category = "Rune")
	FVector RotationAxis = FVector::UpVector;

	UPROPERTY(EditAnywhere, Category = "Interact|Highlight")
	bool bHighlightOnFocus = true;

	UPROPERTY(EditAnywhere, Category = "Interact|Highlight", meta = (EditCondition = "bHighlightOnFocus", ClampMin = "0", ClampMax = "255"))
	int32 HighlightStencilValue = 252;

private:
	void ApplyVisualRotation() const;
	void SetHighlight(bool bEnable) const;
};