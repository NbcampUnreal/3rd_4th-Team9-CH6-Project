#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameplayTagContainer.h"
#include "DoorActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USoundBase;
class USoundAttenuation;

UCLASS()
class REMNANTSOUL_API ADoorActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ADoorActor();

	// IInteractable
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorRequiredTags;

	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorBlockedTags;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void StartRotateTo(const FRotator& NewTarget, bool bTargetOpen);
	void UpdateRotate(float DeltaSeconds);
	float ComputeOpenSignFromDot(AActor* Interactor) const;

	// 태그 게이트
	bool PassesTagGate(AActor* Interactor) const;

	void OnFocusBegin_Implementation(AActor* Interactor);
	void OnFocusEnd_Implementation(AActor* Interactor);

	// SFX
	void PlaySFX(USoundBase* Sound);
	FVector GetSFXLocation() const;

private:
	// ===== Components =====
	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> Hinge;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	// ===== Settings =====
	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bStartOpen = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	float OpenYaw = 90.0f;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bUseDotForDirection = true;

	
	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bOpenAwayFromInteractor = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bInvertChosenDirection = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings", meta=(ClampMin="0.01"))
	float RotateTime = 0.25f;

	// ===== SFX =====
	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_OpenStart = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_OpenEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_CloseStart = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_CloseEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundAttenuation> SFX_Attenuation = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	float SFX_Volume = 1.0f;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	float SFX_Pitch = 1.0f;

	// ===== Runtime =====
	bool bIsOpen = false;
	bool bRotating = false;
	bool bTargetOpenForSFX = false;

	float RotateElapsed = 0.0f;

	FRotator ClosedRot;
	FRotator OpenRot;

	FQuat StartQuat;
	FQuat TargetQuat;
};
