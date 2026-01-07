#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "GameplayTagContainer.h"
#include "RSMetalDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USoundBase;
class USoundAttenuation;

UCLASS()
class REMNANTSOUL_API ARSMetalDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSMetalDoor();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	// ===== SFX =====
	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_OpenStart = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundBase> SFX_OpenEnd = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	TObjectPtr<USoundAttenuation> SFX_Attenuation = nullptr;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	float SFX_Volume = 1.0f;

	UPROPERTY(EditAnywhere, Category="Door|SFX")
	float SFX_Pitch = 1.0f;

	void PlaySFX(USoundBase* Sound) const;
	FVector GetSFXLocation() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void StartMoveTo(bool bOpen);
	void UpdateMove(float DeltaSeconds);
	bool PassesTagGate(AActor* Interactor) const;

private:
	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<USceneComponent> DoorPanel;

	UPROPERTY(VisibleAnywhere, Category="Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	
	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorRequiredTags;

	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorBlockedTags;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bStartOpen = false;

	
	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bToggleable = true;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	FVector OpenOffset = FVector(0.0f, 0.0f, 200.0f);

	UPROPERTY(EditAnywhere, Category="Door|Settings", meta=(ClampMin="0.01"))
	float MoveTime = 0.35f;

	// ===== Runtime =====
	bool bIsOpen = false;
	bool bMoving = false;
	float MoveElapsed = 0.0f;

	FVector ClosedLoc;
	FVector OpenLoc;
};
