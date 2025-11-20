// 1.
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/GameFrameworkComponent.h"
//#include "CVHealthComponent.generated.h"
//
//// 전방선언
//class UCVAbilitySystemComponent;
//class UCVHealthSet; // TODO: 네 AttributeSet
//class AActor;
//
//struct FOnAttributeChangeData;
//
///** 체력 변화 브로드캐스트 */
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCVHealthAttributeChanged, UCVHealthComponent*, HealthComp, float, OldValue, float, NewValue, AActor*, Instigator);
//
//UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
//class CHRONOVEIL_API UCVHealthComponent : public UGameFrameworkComponent
//{
//	GENERATED_BODY()
//
//public:
//	UCVHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	// ASC와 HealthSet 연결/해제
//	void InitializeWithAbilitySystem(UCVAbilitySystemComponent* InASC);
//	void UninitializeWithAbilitySystem();
//
//	// Attribute 변경 콜백
//	void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
//
//	// BP 접근자
//	UFUNCTION(BlueprintPure, Category = "CV|Health")
//	static UCVHealthComponent* FindHealthComponent(const AActor* Actor);
//
//	UFUNCTION(BlueprintCallable, Category = "CV|Health")
//	float GetHealth() const;
//
//	UFUNCTION(BlueprintCallable, Category = "CV|Health")
//	float GetMaxHealth() const;
//
//	UFUNCTION(BlueprintCallable, Category = "CV|Health")
//	float GetHealthNormalized() const;
//
//public:
//	UPROPERTY(BlueprintAssignable)
//	FCVHealthAttributeChanged OnHealthChanged;
//
//protected:
//	UPROPERTY()
//	TObjectPtr<UCVAbilitySystemComponent> AbilitySystemComponent = nullptr;
//
//	UPROPERTY()
//	TObjectPtr<const UCVHealthSet> HealthSet = nullptr; // TODO: 네 HealthSet
//};
