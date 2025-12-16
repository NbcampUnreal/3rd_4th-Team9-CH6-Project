// RSCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "RSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class REMNANTSOUL_API ARSCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ARSCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Component

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UAbilitySystemComponent> ASC;

#pragma endregion

#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

	void HandleGameplayAbilityInputPressed(int32 InInputID);

	void HandleGameplayAbilityInputReleased(int32 InInputID);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> SuperJumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> AttackAction;

#pragma endregion


#pragma region Ability

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TMap<int32, TSubclassOf<UGameplayAbility>> GrantedInputAbilities;

#pragma endregion

#pragma region Attack

public:
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

#pragma endregion

};
