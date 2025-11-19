#pragma once

#include "AbilitySystemInterface.h"
#include "Game/Cosmetics/CVCharacterPartTypes.h"
#include "GameFramework/Character.h"
#include "CVCharacter.generated.h"

class ACVPlayerController;
class ACVPlayerState;
class UCVPawnData;
class UCVCameraComponent;
class UCVHeroComponent;
class UCVCharacterPartComponent;

UCLASS()
class CHRONOVEIL_API ACVCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACVCharacter();
	//ACVCharacter(const FObjectInitializer& ObjectInitializer);


	UFUNCTION(BlueprintCallable, Category = "ACVCharacter")
	ACVPlayerController* GetCVPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "ACVCharacter")
	ACVPlayerState* GetCVPlayerState() const;

	UFUNCTION()
	void OnRep_PawnData();

	UFUNCTION(BlueprintCallable, Category = "ACVCharacter")
	virtual UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;

	const UCVPawnData* GetPawnData() const { return PawnData; }
	//TArray<FCVCharacterPartHandle>& GetCharacterPartHandles() { return CharacterPartHandles; }

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_PawnData, Category = "PawnData")
	TObjectPtr<const UCVPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Character")
	TObjectPtr<UCVCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|Character")
	TObjectPtr<UCVHeroComponent> HeroComponent;

	//UCVCharacterPartComponent* GetCVCharacterPartComponent();

private:
	//TArray<FCVCharacterPartHandle> CharacterPartHandles;

	//void AddInitCharacterPartsAtServer();
	//void SetBodyMeshes();
};
