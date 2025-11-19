#include "Game/Player/CVPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ACVPlayer::ACVPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	// CDO
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	SpringArm->SetupAttachment(FirstPersonMesh, FName("S_Camera"));
	Camera->SetupAttachment(SpringArm);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));

	Tags.Add(FName("Player"));
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	Camera->bUsePawnControlRotation = true;
	Camera->bEnableFirstPersonFieldOfView = true;
	Camera->bEnableFirstPersonScale = true;
}

void ACVPlayer::BeginPlay()
{
	Super::BeginPlay();

	//FirstPersonMesh->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);

}

void ACVPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACVPlayer, AimDirection);
}

void ACVPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}
