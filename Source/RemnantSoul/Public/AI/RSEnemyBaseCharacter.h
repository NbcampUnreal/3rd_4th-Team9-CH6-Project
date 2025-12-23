#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RSEnemyBaseCharacter.generated.h"

class UAbilitySystemComponent;
class URSEnemyBaseAttributeSet;

UCLASS()
class REMNANTSOUL_API ARSEnemyBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	/* Ability System Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	/* AttributeSet */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<URSEnemyBaseAttributeSet> EnemyBasicAttributeSet;

protected:
	/* 초기 어빌리티 배열 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;
public:
	ARSEnemyBaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* 어빌리티 부여 함수 */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant);

	/* 어빌리티 제거 함수 */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);
	
	/* 어빌리티 변경 알림 함수 */
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void SendAbilitiesChangedEvent();

protected:
	virtual void PossessedBy(AController* NewController) override;
};
