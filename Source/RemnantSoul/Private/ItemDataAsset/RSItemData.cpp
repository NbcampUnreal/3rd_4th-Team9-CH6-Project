// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataAsset/RSItemData.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

bool URSItemData::TryUse(AActor* User) const
{
	if (!User)
	{
		return false;
	}

	bool bSuccess = false;

	if (UseGameplayEffect)
	{
		bSuccess = ApplyGameplayEffectToUser(User);
	}
	else
	{
		bSuccess = false; 
	}

	if (bSuccess)
	{
		PlayUseSFX(User);
	}

	return bSuccess;
}

void URSItemData::PlayUseSFX(AActor* User) const
{
	if (!User) return;

	UWorld* World = User->GetWorld();
	if (!World || World->GetNetMode() == NM_DedicatedServer) return;

	if (UseSFX.IsNull()) return;

	USoundBase* Sound = UseSFX.LoadSynchronous();
	if (!Sound) return;

	if (bPlayUseSFX2D)
	{
		UGameplayStatics::PlaySound2D(World, Sound, UseSFXVolume, UseSFXPitch);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, User->GetActorLocation(), UseSFXVolume, UseSFXPitch);
	}
}

bool URSItemData::ApplyGameplayEffectToUser(AActor* User) const
{
	if (!UseGameplayEffect)
	{
		return false;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(User);
	if (!ASI)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	const FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(UseGameplayEffect, 1.0f, Context);
	if (!Spec.IsValid())
	{
		return false;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	return true;
}
