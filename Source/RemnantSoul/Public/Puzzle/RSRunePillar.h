#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "RSRunePillar.generated.h"

class USkeletalMeshComponent;
class URSRunePillarAnimInstance;
class USoundBase;

UCLASS()
class REMNANTSOUL_API ARSRunePillar : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSRunePillar();

	// IInteractable
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void OnFocusBegin_Implementation(AActor* Interactor) override;
	virtual void OnFocusEnd_Implementation(AActor* Interactor) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar")
	TObjectPtr<USoundBase> RotateSFX;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar")
	TObjectPtr<USoundBase> SolvedSFX;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar", meta=(ClampMin="0.0"))
	float RotateSFXVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar", meta=(ClampMin="0.0"))
	float RotateSFXPitch = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar", meta=(ClampMin="0.0"))
	float SolvedSFXVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar", meta=(ClampMin="0.0"))
	float SolvedSFXPitch = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar")
	bool bRotateSFX2D = false;

	UPROPERTY(EditDefaultsOnly, Category="Audio|RunePillar")
	bool bSolvedSFX2D = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Puzzle")
	void BP_OnSolved(AActor* Interactor);

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(EditAnywhere, Category="Puzzle", meta=(ClampMin="1"))
	int32 MaxState = 4;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	float StepDegrees = 90.f;

	UPROPERTY(VisibleInstanceOnly, Category="Puzzle")
	int32 TopState = 0;

	UPROPERTY(VisibleInstanceOnly, Category="Puzzle")
	int32 MidState = 0;

	UPROPERTY(VisibleInstanceOnly, Category="Puzzle")
	int32 BotState = 0;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	int32 TargetTop = 2;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	int32 TargetMid = 3;

	UPROPERTY(EditAnywhere, Category="Puzzle")
	int32 TargetBot = 1;

	UPROPERTY(VisibleInstanceOnly, Category="Puzzle")
	bool bSolved = false;

	UPROPERTY(EditAnywhere, Category="Interact|Highlight")
	bool bHighlightOnFocus = true;

	UPROPERTY(EditAnywhere, Category="Interact|Highlight", meta=(ClampMin="0", ClampMax="255", EditCondition="bHighlightOnFocus"))
	int32 HighlightStencilValue = 252;


	URSRunePillarAnimInstance* GetRuneAnim() const;
	void ApplyAnimRotations() const;

	bool TryResolveHitBone(AActor* Interactor, FName& OutBone) const;
	void AdvanceByBone(const FName& Bone);

	UPROPERTY(EditAnywhere, Category="Interact|Trace", meta=(ClampMin="1.0"))
	float TraceDistance = 900.f;

	UPROPERTY(EditAnywhere, Category="Interact|Trace")
	bool bTraceComplex = false;
	bool IsSolved() const;
	
	UPROPERTY(EditInstanceOnly, Category="Puzzle|FogWall")
	TObjectPtr<AActor> FogWallActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Puzzle|FogWall")
	bool bHideFogWallOnSolved = true;

	UPROPERTY(EditAnywhere, Category="Puzzle|FogWall")
	bool bDisableFogCollisionOnSolved = true;


	void OpenFogWall() const;

	void SetHighlight(bool bEnable) const;

	bool ShouldPlayLocalSFX(AActor* Interactor) const;
	void PlaySFX(AActor* Interactor, USoundBase* Sound, float Volume, float Pitch, bool b2D) const;

	static const FName BoneTop; 
	static const FName BoneMid; 
	static const FName BoneBot;
};
