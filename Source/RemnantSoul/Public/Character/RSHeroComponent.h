// RSHeroComponent.h
#pragma once

#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "RSHeroComponent.generated.h"

class APlayerController;
class UInputComponent;
class ARSCharacter;
class UEnhancedInputLocalPlayerSubsystem;
class URSInputConfig;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

protected:
	void InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController);

	void Input_AbilityTagPressed(FGameplayTag InputTag);
	void Input_AbilityTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Interaction(const FInputActionValue& Value);
	void Input_InventoryToggle(const FInputActionValue& Value);
	
	ARSCharacter* GetOwnerCharacter() const;
};
