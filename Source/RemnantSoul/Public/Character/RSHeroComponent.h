#pragma once

#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "RSHeroComponent.generated.h"

class APlayerController;
class UInputMappingContext;
class UInputComponent;
class ARSCharacter;
class ARSPlayerState;
class UEnhancedInputLocalPlayerSubsystem;
class URSEnhancedInputComponent;
class URSInputConfig;
class URSEquipManagerComponent;
class URSCombatStyleData;

DECLARE_MULTICAST_DELEGATE(FRSOnInputReady);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSHeroComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

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
	void Input_EquipSlot1();
	void Input_EquipSlot2();


	
	
	ARSCharacter* GetOwnerCharacter() const;

protected:
	UPROPERTY()
	TObjectPtr<URSEquipManagerComponent> EquipManager;

private:
	UPROPERTY()
	TObjectPtr<const URSInputConfig> CurrentOverlayConfig = nullptr;

	UPROPERTY()
	TArray<uint32> BaseAbilityBindHandles;

	UPROPERTY()
	TArray<uint32> OverlayBindHandles;

	UPROPERTY()
	TArray<TWeakObjectPtr<UInputMappingContext>> OverlayAddedIMCs;

private:
	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;
	URSEnhancedInputComponent* GetRSEnhancedInputComponent() const;

	void LogAbilityBindings(const URSInputConfig* Config, const TCHAR* Label) const;


public:
	void ApplyOverlayInputConfig(const URSInputConfig* Overlay);
	void ClearOverlayInputConfig();

#pragma region Delegate
public:
	FRSOnInputReady OnInputReady;

private:
	bool bInputInitialized = false;
	bool bSetupPICAcknowledged = false;
#pragma endregion

#pragma region CombatStyle
public:
	void ApplyCombatStyle(const URSCombatStyleData* NewStyle);

#pragma endregion

};
