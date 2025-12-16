// RSHealPack.h

#pragma once

#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "RSHealPack.generated.h"

class UParticleSystemComponent;
class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class REMNANTSOUL_API ARSHealPack : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARSHealPack();

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void OnActiveSmokeTimerElapsed();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ARSHealPack|Component")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, Category = "ARSHealPack|Component")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(EditDefaultsOnly, Category = "ARSHealPack|Component")
	TObjectPtr<UParticleSystemComponent> Steam;

	UPROPERTY(EditAnywhere, Category = "ARSHealPack|SteamParticle")
	float ActiveSteamParticlePeriod;

	UPROPERTY(EditDefaultsOnly, Category = "ARSHealPack|GameplayAbilitySystem")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, Category = "ARSHealPack|GameplayAbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	FTimerHandle ActiveSteamParticleTimer;

};
