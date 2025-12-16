// RSHealPack.cpp


#include "Gimmick/RSHealPack.h"
#include "Particles/ParticleSystemComponent.h"
#include "RemnantSoul/RemnantSoul.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GAS/GA/RSGameplayAbility_ActiveParticle.h"
#include "RemnantSoul/RSGameplayTags.h"

ARSHealPack::ARSHealPack()
	: ActiveSteamParticlePeriod(5.f)
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	Steam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Steam"));
	Steam->SetupAttachment(Root);

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

void ARSHealPack::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Steam->bAutoActivate = false;
	Steam->Deactivate();

	ASC->InitAbilityActorInfo(this, this);
	//FGameplayAbilitySpec AbilitySpec(URSGameplayAbility_ActiveParticle::StaticClass());
	//ASC->GiveAbility(AbilitySpec);

	for (const auto& GrantedAbility : GrantedAbilities)
	{
		FGameplayAbilitySpec GrantedAbilitySpec(GrantedAbility);
		ASC->GiveAbility(GrantedAbilitySpec);
	}
}

void ARSHealPack::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ActiveSteamParticleTimer, this, &ThisClass::OnActiveSmokeTimerElapsed, ActiveSteamParticlePeriod, true, 0.0f);
}

void ARSHealPack::OnActiveSmokeTimerElapsed()
{
	UE_LOG(LogTemp, Log, TEXT("Begin"));

	// if (Steam->IsActive() == false)
	// {
	// 	Steam->Activate(true);
	// }
	// else
	// {
	// 	Steam->Deactivate();
	// }

	// FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(URSGameplayAbility_ActiveParticle::StaticClass());
	// if (nullptr == AbilitySpec)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("No Rotate Spec Found."));
	// 	return;
	// }
	//    
	// if (AbilitySpec->IsActive() == false)
	// {
	// 	ASC->TryActivateAbility(AbilitySpec->Handle);
	// }
	// else
	// {
	// 	ASC->CancelAbilityHandle(AbilitySpec->Handle);
	// }

	FGameplayTagContainer TargetTag(ABILITY_COSMETIC_STEAMPARTICLE);

	if (ASC->HasMatchingGameplayTag(STATE_ACTIVATED_STEAMPARTICLE) == false)
	{
		ASC->TryActivateAbilitiesByTag(TargetTag);
	}
	else
	{
		ASC->CancelAbilities(&TargetTag);
	}
}

UAbilitySystemComponent* ARSHealPack::GetAbilitySystemComponent() const
{
	return ASC;
}