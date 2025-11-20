// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Devices/CVDevice_Base.h"
#include "CVDevice_AffectionDoll.generated.h"

class USphereComponent;

/**
 * 애정인형 디바이스
 * - 스폰 시 바닥에서 커지듯이 스케일 업
 * - 짧은 시간 동안 주변 적 캐릭터를 인형 중심으로 끌어당김
 */
UCLASS()
class CHRONOVEIL_API ACVDevice_AffectionDoll : public ACVDevice_Base
{
	GENERATED_BODY()

public:
	ACVDevice_AffectionDoll();

protected:
	// 인형이 바닥에서 커지는 데 걸리는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Grow")
	float GrowDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Pull")
	float PullDuration = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Pull")
	float PullRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Pull")
	float PullStrength = 2000.f;

	// 커지기전에 시작 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Grow")
	FVector StartScale = FVector(0.2f, 0.2f, 0.2f);

	// 다 커진 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AffectionDoll|Grow")
	FVector TargetScale = FVector(1.0f, 1.0f, 1.0f);

	float SpawnTime = 0.f;
	bool bIsGrowing = true;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 주변 캐릭터를 인형 중심으로 끌어당김
	void ApplyPull(float DeltaTime);
};
