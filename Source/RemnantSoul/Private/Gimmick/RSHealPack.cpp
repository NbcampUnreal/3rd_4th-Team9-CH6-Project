// RSHealPack.cpp


#include "Gimmick/RSHealPack.h"
#include "Particles/ParticleSystemComponent.h"
#include "RemnantSoul.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GAS/GA/RSGameplayAbility_ActiveParticle.h"
#include "RSGameplayTags.h"

ARSHealPack::ARSHealPack()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

void ARSHealPack::PostInitializeComponents()
{
	Super::PostInitializeComponents();



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

	
}



UAbilitySystemComponent* ARSHealPack::GetAbilitySystemComponent() const
{
	return ASC;
}