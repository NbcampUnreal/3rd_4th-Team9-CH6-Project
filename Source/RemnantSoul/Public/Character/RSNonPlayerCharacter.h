#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RSNonPlayerCharacter.generated.h"

class UAbilitySystemComponent;
class URSAttributeSet_Character;
class UGameplayEffect;
class URSWidgetComponent;

UCLASS()
class REMNANTSOUL_API ARSNonPlayerCharacter
	: public ACharacter
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARSNonPlayerCharacter();

	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	virtual void OnOutOfHealth();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<URSAttributeSet_Character> AttributeSet;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<UGameplayEffect> InitStatEffect;

	UPROPERTY(EditAnywhere, Category = GAS)
	float Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSWidgetComponent> HPBar;

};