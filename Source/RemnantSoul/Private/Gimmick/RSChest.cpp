// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RSChest.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTags.h"		

// Sets default values
ARSChest::ARSChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger -> SetBoxExtent((FVector(40.0f, 42.0f, 30.0f)));
	SetRootComponent(Trigger);
	
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh -> SetupAttachment(Trigger);
	Mesh -> SetCollisionProfileName(TEXT("NoCollision"));
	
	//GameplayCueTag =	

}

UAbilitySystemComponent* ARSChest::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARSChest::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	
	InvokeGameplayCue(Other);
	ApplyEffectToTarget(Other);
	
	Mesh -> SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(2.0f);
	
}

void ARSChest::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ASC-> InitAbilityActorInfo(this, this);
	
}

void ARSChest::ApplyEffectToTarget(AActor* Target)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC)
	{
		FGameplayEffectContextHandle EffectContext = TargetASC -> MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = TargetASC -> MakeOutgoingSpec(GameplayEffectClass, 1 ,EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			TargetASC -> BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}


void ARSChest::InvokeGameplayCue(AActor* Target)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator =  Target;
	Param. Location = GetActorLocation();
	ASC -> ExecuteGameplayCue(GameplayCueTag, Param);
}
