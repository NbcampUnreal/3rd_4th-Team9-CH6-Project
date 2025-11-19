#include "CVPlayerAnimInstance.h"
#include "CVPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

void UCVPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CVPlayer = Cast<ACVPlayer>(TryGetPawnOwner());

	if (CVPlayer)
		MovementComponent = CVPlayer->GetCharacterMovement();
}

void UCVPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CVPlayer == nullptr)
		return;

	Velocity = CVPlayer->GetVelocity();
	GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

	bool IsAccelerating = (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
	ShouldMove = IsAccelerating && (GroundSpeed > 3);

	MovementDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, CVPlayer->GetActorRotation());

	AimDirection = CVPlayer->AimDirection;
}