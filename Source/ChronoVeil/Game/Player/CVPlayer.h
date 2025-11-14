// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CVPlayer.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class ACVWeaponBase;

UCLASS()
class CHRONOVEIL_API ACVPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ACVPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	double AimDirection;

	UPROPERTY(Replicated)
	ACVWeaponBase* CurrentWeapon;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
};
