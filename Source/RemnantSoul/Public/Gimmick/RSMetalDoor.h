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

	// ==== Lever-only API ====
	UFUNCTION(BlueprintCallable, Category="Door|Lever")
	void OpenByLever();

	UFUNCTION(BlueprintCallable, Category="Door|Lever")
	void CloseByLever();

	UFUNCTION(BlueprintCallable, Category="Door|Lever")
	void ToggleByLever();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// ==== Move ====
	void StartMoveTo(bool bOpen);
	void UpdateMove(float DeltaSeconds);

	// ==== Tag Gate  ====
	bool PassesTagGate(AActor* Interactor) const;

	// ==== SFX ====
	void PlaySFX(USoundBase* Sound) const;
	FVector GetSFXLocation() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Door|Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Door|Components")
	TObjectPtr<USceneComponent> DoorPanel = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Door|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh = nullptr;

	// ==== Tag Gate ====
	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorRequiredTags;

	UPROPERTY(EditAnywhere, Category="Door|TagGate")
	FGameplayTagContainer InteractorBlockedTags;

	// ==== Settings ====
	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bLocked = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bStartOpen = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	FVector OpenOffset = FVector(0, 0, 200);

	UPROPERTY(EditAnywhere, Category="Door|Settings")
	bool bToggleable = false;

	UPROPERTY(EditAnywhere, Category="Door|Settings", meta=(ClampMin="0.01"))
	float MoveTime = 0.25f;

	// ==== SFX ====
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

	// ==== Runtime ====
	bool bIsOpen = false;
	bool bMoving = false;
	float MoveElapsed = 0.0f;

	FVector ClosedLoc;
	FVector OpenLoc;
};