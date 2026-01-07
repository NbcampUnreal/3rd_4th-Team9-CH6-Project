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

	UPROPERTY(EditAnywhere, Category="SFX")
	TObjectPtr<USoundBase> SFX_OpenStart = nullptr;

	UPROPERTY(EditAnywhere, Category="SFX")
	TObjectPtr<USoundBase> SFX_OpenEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="SFX")
	TObjectPtr<USoundBase> SFX_CloseStart = nullptr;

	UPROPERTY(EditAnywhere, Category="SFX")
	TObjectPtr<USoundBase> SFX_CloseEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="SFX")
	TObjectPtr<USoundAttenuation> SFX_Attenuation = nullptr;

	UPROPERTY(EditAnywhere, Category="SFX")
	float SFX_Volume = 1.0f;

	UPROPERTY(EditAnywhere, Category="SFX")
	float SFX_Pitch = 1.0f;

	bool bTargetOpenForSFX = false;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void PlaySFX(USoundBase* Sound);
	FVector GetSFXLocation() const;

private:
	void StartRotateTo(const FRotator& NewTarget);
	void UpdateRotate(float DeltaSeconds);
	float ComputeOpenSignFromDot(AActor* Interactor) const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> Root;

	// 힌지(회전축). BP에서 위치를 경첩으로 옮기면 피벗 문제 해결됨.
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> Hinge;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	//
	
	bool PassesTagGate(AActor* Interactor) const;
	void OnFocusBegin_Implementation(AActor* Interactor);
	void OnFocusEnd_Implementation(AActor* Interactor);

	// ===== Settings =====
	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	bool bStartOpen = false;

	
	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	float OpenYaw = 90.0f;

	
	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	bool bUseDotForDirection = true;

	
	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	bool bOpenAwayFromInteractor = true;

	
	UPROPERTY(EditAnywhere, Category = "Door|Settings")
	bool bInvertChosenDirection = false;

	UPROPERTY(EditAnywhere, Category = "Door|Settings", meta = (ClampMin = "0.01"))
	float RotateTime = 0.25f;

	
	
	// ===== Runtime =====
	bool bIsOpen = false;
	bool bRotating = false;
	float RotateElapsed = 0.0f;

	FRotator ClosedRot;
	FRotator OpenRot; 

	FQuat StartQuat;
	FQuat TargetQuat;
};
