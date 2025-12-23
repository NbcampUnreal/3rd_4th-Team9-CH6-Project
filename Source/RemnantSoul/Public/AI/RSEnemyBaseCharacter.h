#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RSEnemyBaseCharacter.generated.h"

struct FGameplayTag;
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
	/* 생성자 */
	ARSEnemyBaseCharacter();

	/* Ability System Component 반환 함수 */
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
	/* Controller가 Pawn에 빙의할 때 호출되는 함수 */
	virtual void PossessedBy(AController* NewController) override;

	/* IsDead 태그가 변경되었을 때 호출되는 함수 */
	virtual void OnDeathTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	/* 죽음 처리 함수 (블루프린트에서 재정의하지 않으면 C++ 내용이 Default로 적용) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void HandleDeath();
};
