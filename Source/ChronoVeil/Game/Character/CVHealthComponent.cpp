// 1.
//#include "Game/Character/CVHealthComponent.h"
//
//// TODO: 준비되면 주석 해제
//// #include "AbilitySystem/CVAbilitySystemComponent.h"
//// #include "AbilitySystem/Attributes/CVHealthSet.h"
//// #include "GameplayEffectExtension.h"
//#include "GameFramework/Actor.h"
//
//UCVHealthComponent::UCVHealthComponent(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryComponentTick.bCanEverTick = false;
//	PrimaryComponentTick.bStartWithTickEnabled = false;
//}
//
//void UCVHealthComponent::InitializeWithAbilitySystem(UCVAbilitySystemComponent* InASC)
//{
//	AActor* Owner = GetOwner();
//	if (!Owner) return;
//
//	if (AbilitySystemComponent)
//	{
//		// 이미 초기화됨
//		return;
//	}
//
//	AbilitySystemComponent = InASC;
//	if (!AbilitySystemComponent)
//	{
//		return;
//	}
//
//	// HealthSet = AbilitySystemComponent->GetSet<UCVHealthSet>();
//	// if (!HealthSet) return;
//
//	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCVHealthSet::GetHealthAttribute())
//	//	.AddUObject(this, &ThisClass::HandleHealthChanged);
//
//	// 초기 브로드캐스트
//	// OnHealthChanged.Broadcast(this, 0.f, HealthSet->GetHealth(), nullptr);
//}
//
//void UCVHealthComponent::UninitializeWithAbilitySystem()
//{
//	AbilitySystemComponent = nullptr;
//	HealthSet = nullptr;
//}
//
//void UCVHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
//{
//	// AActor* Instigator = ChangeData.GEModData ? ChangeData.GEModData->EffectSpec.GetEffectContext().GetOriginalInstigator() : nullptr;
//	// OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);
//}
//
//UCVHealthComponent* UCVHealthComponent::FindHealthComponent(const AActor* Actor)
//{
//	return (Actor ? Actor->FindComponentByClass<UCVHealthComponent>() : nullptr);
//}
//
//float UCVHealthComponent::GetHealth() const
//{
//	// return (HealthSet ? HealthSet->GetHealth() : 0.f);
//	return 0.f;
//}
//
//float UCVHealthComponent::GetMaxHealth() const
//{
//	// return (HealthSet ? HealthSet->GetMaxHealth() : 0.f);
//	return 0.f;
//}
//
//float UCVHealthComponent::GetHealthNormalized() const
//{
//	// if (HealthSet)
//	// {
//	// 	const float H = HealthSet->GetHealth();
//	// 	const float M = HealthSet->GetMaxHealth();
//	// 	return (M > 0.f) ? (H / M) : 0.f;
//	// }
//	return 0.f;
//}
