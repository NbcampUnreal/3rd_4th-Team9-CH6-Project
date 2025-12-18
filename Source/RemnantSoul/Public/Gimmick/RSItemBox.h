#pragma once

#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "RSItemBox.generated.h"

class UAbilitySystemComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UGameplayEffect;

UCLASS()
class REMNANTSOUL_API ARSItemBox
	: public AActor
	, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARSItemBox();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void NotifyActorBeginOverlap(AActor* Other) override;

protected:
	virtual void PostInitializeComponents() override;

	void ApplyEffectToTarget(AActor* Target);

	void InvokeGameplayCue(AActor* Target);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;

};
