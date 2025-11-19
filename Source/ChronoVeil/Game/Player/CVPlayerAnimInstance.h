#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CVPlayerAnimInstance.generated.h"

class ACVPlayer;
class UCharacterMovementComponent;

UCLASS()
class CHRONOVEIL_API UCVPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACVPlayer> CVPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double AimDirection;
};