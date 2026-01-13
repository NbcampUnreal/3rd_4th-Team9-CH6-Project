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
class URSInputConfig;

DECLARE_MULTICAST_DELEGATE(FRSOnInputReady);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSHeroComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "RS|Hero|Input")
	bool IsInputInitialized() const { return bInputInitialized; }

protected:
	void InitializePlayerInput(UInputComponent* PlayerInputComponent, APlayerController* PlayerController);

	void Input_AbilityTagPressed(FGameplayTag InputTag);
	void Input_AbilityTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_InventoryToggle(const FInputActionValue& Value);

	void Input_EquipSlot1();
	void Input_EquipSlot2();

	void Input_QuickSlotCycle(const FInputActionValue& Value);
	void Input_QuickSlotUse(const FInputActionValue& Value);
	
	ARSCharacter* GetOwnerCharacter() const;

	void HandleEquipInput(FGameplayTag InputTag);

	// Base Ability 바인딩만 제거 (Overlay는 별도)
	void ClearBaseAbilityBindings();

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

	UPROPERTY(Transient)
	TWeakObjectPtr<UInputComponent> LastSetupInputComponent;

private:
	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;
	URSEnhancedInputComponent* GetRSEnhancedInputComponent() const;

	void LogAbilityBindings(const URSInputConfig* Config, const TCHAR* Label) const;

#pragma region Debug
private:
	void DebugDumpInputState(const URSInputConfig* InputConfig, const TCHAR* Label) const; 
#pragma endregion

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


#pragma endregion

};
