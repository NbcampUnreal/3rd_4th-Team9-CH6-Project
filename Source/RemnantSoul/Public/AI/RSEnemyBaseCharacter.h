#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RSEnemyBaseCharacter.generated.h"

class UAbilitySystemComponent;
class URSEnemyBasicAttributeSet;

UCLASS()
class REMNANTSOUL_API ARSEnemyBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARSEnemyBaseCharacter();
	
	/* Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	/* AttributeSet */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<URSEnemyBasicAttributeSet> EnemyBasicAttributeSet;
	
protected:
	virtual void PossessedBy(AController* NewController) override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
