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

	WeaponType = EWeaponType::Pistol;
	MaxAmmo = 12;
	CurrentAmmo = MaxAmmo;
}

void ACVWeaponBase::OnRep_CurrentAmmo()
{
	UE_LOG(LogTemp, Verbose, TEXT("OnRep_CurrentAmmo: %d"), CurrentAmmo);
}

void ACVWeaponBase::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
		return;
	}

	ServerFire_Implementation();
}

void ACVWeaponBase::ServerFire_Implementation()
{
	if (WeaponType == EWeaponType::Knife)
	{
		MulticastPlayFire();
		return;
	}

	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon out of ammo"));
		return;
	}

	--CurrentAmmo;

	MulticastPlayFire();
}

void ACVWeaponBase::MulticastPlayFire_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Weapon fired: %d (Type: %d)"), CurrentAmmo, static_cast<uint8>(WeaponType));
}

void ACVWeaponBase::OnPickupAreaBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (!OverlappingPawn)
	{
		return;
	}

	ACharacter* PlayerChar = Cast<ACharacter>(OverlappingPawn);
	if (!PlayerChar)
	{
		return;
	}

	ACVPlayer* CVPlayer = Cast<ACVPlayer>(PlayerChar);
	if (!CVPlayer)
	{
		return;
	}

	CVPlayer->CurrentWeapon = this;

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(PlayerChar->GetMesh(), Rules, FName("S_Rifle"));

	PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogTemp, Log, TEXT("Weapon picked up by player: %s"), *CVPlayer->GetName());
}

void ACVWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACVWeaponBase, CurrentAmmo);
}