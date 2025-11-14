// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Weapon/CVWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Game/Player/CVPlayer.h"
#include "GameFramework/Character.h"

ACVWeaponBase::ACVWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = Mesh;

	PickupArea = CreateDefaultSubobject<USphereComponent>(TEXT("PickupArea"));
	PickupArea->SetupAttachment(RootComponent);
	PickupArea->SetSphereRadius(100.f);
	PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupArea->OnComponentBeginOverlap.AddDynamic(this, &ACVWeaponBase::OnPickupAreaBegin);

	// sensible defaults
	WeaponType = EWeaponType::Pistol;
	MaxAmmo = 12;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponBase::OnRep_CurrentAmmo()
{
	// RepNotify: could update UI or play reload animations here
	UE_LOG(LogTemp, Verbose, TEXT("OnRep_CurrentAmmo: %d"), CurrentAmmo);
}

void ACVWeaponBase::Fire()
{
	// Clients should call ServerFire. Server will perform validation and multicast the effect.
	if (!HasAuthority())
	{
		ServerFire();
		return;
	}

	// If server, perform the same logic directly
	ServerFire_Implementation();
}

void ACVWeaponBase::ServerFire_Implementation()
{
	// Simple server-side firing logic
	if (WeaponType == EWeaponType::Knife)
	{
		// Melee attack - no ammo consumption
		MulticastPlayFire();
		return;
	}

	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon out of ammo"));
		return;
	}

	--CurrentAmmo;

	// Broadcast visual/sound effects to all clients
	MulticastPlayFire();
}

void ACVWeaponBase::MulticastPlayFire_Implementation()
{
	// Play simple debug message for now. Replace with animations / sounds later.
	UE_LOG(LogTemp, Log, TEXT("Weapon fired: %d (Type: %d)"), CurrentAmmo, static_cast<uint8>(WeaponType));
}

void ACVWeaponBase::OnPickupAreaBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OtherActor)
		return;

	// Check if overlapping actor is a pawn
	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (!OverlappingPawn)
		return;

	// Try to find player character
	ACharacter* PlayerChar = Cast<ACharacter>(OverlappingPawn);
	if (!PlayerChar)
		return;

	// Attempt to find ACVPlayer on Pawn
	ACVPlayer* CVPlayer = Cast<ACVPlayer>(PlayerChar);
	if (!CVPlayer)
		return;

	// Give weapon to player: for simplicity, set CurrentWeapon pointer directly and attach to player's mesh
	CVPlayer->CurrentWeapon = this;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(PlayerChar->GetMesh(), Rules, FName("S_Rifle"));

	// Disable pickup collision
	PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Optionally hide in-world mesh or mark as picked up
	UE_LOG(LogTemp, Log, TEXT("Weapon picked up by player: %s"), *CVPlayer->GetName());
}

void ACVWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACVWeaponBase, CurrentAmmo);
}