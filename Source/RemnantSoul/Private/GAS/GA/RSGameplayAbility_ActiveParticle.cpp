// RSGameplayAbility_ActiveParticle.cpp


#include "GAS/GA/RSGameplayAbility_ActiveParticle.h"
#include "Particles/ParticleSystemComponent.h"
#include "RSGameplayTags.h"

URSGameplayAbility_ActiveParticle::URSGameplayAbility_ActiveParticle()
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	AbilityTags.AddTag(Tags.Ability_Cosmetic_SteamParticle);
	ActivationOwnedTags.AddTag(Tags.State_Activated_SteamParticle);
}

void URSGameplayAbility_ActiveParticle::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		UParticleSystemComponent* Particle = Cast<UParticleSystemComponent>(AvatarActor->GetComponentByClass(UParticleSystemComponent::StaticClass()));
		if (IsValid(Particle) == true)
		{
			Particle->Activate(true);
		}
	}
}

void URSGameplayAbility_ActiveParticle::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		UParticleSystemComponent* Particle = Cast<UParticleSystemComponent>(AvatarActor->GetComponentByClass(UParticleSystemComponent::StaticClass()));
		if (IsValid(Particle) == true)
		{
			Particle->Deactivate();
		}
	}
}
