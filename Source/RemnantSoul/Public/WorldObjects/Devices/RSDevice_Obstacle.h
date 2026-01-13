#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/Devices/RSDevice_Base.h"
#include "RSDevice_Obstacle.generated.h"

UCLASS()
class REMNANTSOUL_API ARSDevice_Obstacle : public ARSDevice_Base
{
	GENERATED_BODY()

public:
	ARSDevice_Obstacle();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// 얼마나 아래에서 시작할지(지면 아래로 묻힌 상태)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Obstacle|Rise")
	float RiseStartDepth = 180.0f;

	// 상승 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Obstacle|Rise", meta = (ClampMin = "0.01"))
	float RiseDuration = 0.25f;

	// 올라오는 동안 충돌을 끌지(권장)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Obstacle|Rise")
	bool bDisableCollisionWhileRising = true;

	// 상승 커브 느낌(단순 이징)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Obstacle|Rise")
	bool bUseEaseOut = true;

private:
	bool bRising = false;
	float RiseElapsed = 0.f;

	FVector RiseStartLoc = FVector::ZeroVector;
	FVector RiseTargetLoc = FVector::ZeroVector;

	void BeginRise();
	void FinishRise();
	float EvalAlpha(float LinearAlpha) const;
};
