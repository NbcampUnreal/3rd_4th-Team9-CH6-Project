#include "Game/Character/CVCharacter.h"
#include "Game/Character/CVHeroComponent.h"
#include "Game/Character/CVPawnData.h"
#include "Game/Player/CVPlayerState.h"
#include "Game/Player/CVPlayerController.h"
#include "Game/Camera/CVCameraComponent.h"
#include "Game/Cosmetics/CVPawnComponent_CharacterParts.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
#include "Game/Input/CVEnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

ACVCharacter::ACVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	HeroComponent = CreateDefaultSubobject<UCVHeroComponent>(TEXT("HeroComponent"));

	CameraComponent = CreateDefaultSubobject<UCVCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
}


ACVPlayerController* ACVCharacter::GetCVPlayerController() const
{
	return GetController<ACVPlayerController>();
}


ACVPlayerState* ACVCharacter::GetCVPlayerState() const
{
	return GetPlayerState<ACVPlayerState>();
}

void ACVCharacter::OnRep_PawnData()
{
	if (IsValid(HeroComponent))
	{
		//HeroComponent->TryBindCameraMode(); // 이미 HeroComponent에서 적용함.
		// 필요하면 Input 재바인딩 트리거도 여기서 호출 가능함!
	}
}

UCVAbilitySystemComponent* ACVCharacter::GetCVAbilitySystemComponent() const
{
	return Cast<UCVAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ACVCharacter::GetAbilitySystemComponent() const
{
	ACVPlayerState* CVPlayerState = GetCVPlayerState();
	if (IsValid(CVPlayerState))
	{
		return CVPlayerState->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ACVCharacter::BeginPlay()
{
	Super::BeginPlay();
	//SetBodyMeshes();
}

void ACVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsValid(HeroComponent))
	{
		HeroComponent->SetupPlayerInputComponent(PlayerInputComponent);
	}
}

void ACVCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, PawnData, COND_InitialOnly);
}

void ACVCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//SetBodyMeshes();
	//AddInitCharacterPartsAtServer();
}

//UCVCharacterPartComponent* ACVCharacter::GetCVCharacterPartComponent()
//{
//	UActorComponent* FindComponent = GetComponentByClass(UCVCharacterPartComponent::StaticClass());
//	UCVCharacterPartComponent* PartComponent = Cast<UCVCharacterPartComponent>(FindComponent);
//	return PartComponent;
//}
//
//void ACVCharacter::AddInitCharacterPartsAtServer()
//{
//	UCVCharacterPartComponent* PartComponent = GetCVCharacterPartComponent();
//	if (IsValid(PartComponent))
//	{
//		if (IsValid(PawnData))
//		{
//			PartComponent->AddInitCharacterParts(PawnData->InitCharacterParts);
//		}
//	}
//}
//
//void ACVCharacter::SetBodyMeshes()
//{
//	UCVCharacterPartComponent* PartComponent = GetCVCharacterPartComponent();
//	if (IsValid(PartComponent))
//	{
//		if (IsValid(PawnData))
//		{
//			PartComponent->SetBodyMeshes(PawnData->InitBodyMeshes);
//		}
//	}
//}
