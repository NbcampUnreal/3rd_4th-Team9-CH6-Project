// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GXChest.generated.h"

class UAbilitySystemComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UGameplayEffect;


UCLASS()
class REMNANTSOUL_API AGXChest : 
public AActor,
public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGXChest();

	virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual  void NotifyActorBeginOverlap(AActor* Other) override;
	
protected:
	virtual void PostInitializeComponents() override;

	void ApplyEffectToTarget(AActor* Target);
	
	void InvokeGameplayCue(AActor* Target);
	
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;


public:	
	

};
