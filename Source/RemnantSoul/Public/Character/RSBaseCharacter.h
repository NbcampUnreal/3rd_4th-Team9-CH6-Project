#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "RSBaseCharacter.generated.h"

class UAbilitySystemComponent;
class URSAttributeSet;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class REMNANTSOUL_API ARSBaseCharacter
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARSBaseCharacter();

	/* GAS */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* === PlayerController에서 호출할 공개 인터페이스 === */
	void HandleMoveInput(const FVector2D& MoveVector);
	void HandleLookInput(const FVector2D& LookVector);
	void HandleAttackInput();
	void RollInput();


	void HandleDeath();

	FVector GetLastMovementInput() const { return LastMovementInput; }

protected:
	virtual void BeginPlay() override;

	/* Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* Camera;

	/* GAS */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URSAttributeSet> AttributeSet;
	

	bool bAbilitiesInitialized = false;
	bool bIsDead = false;

	/* GAS 내부 처리 */
	void InitializeAbilities();

	FVector LastMovementInput;
	//Roll 어빌리티에서 사용할 방향 좌표 저장용.


};
