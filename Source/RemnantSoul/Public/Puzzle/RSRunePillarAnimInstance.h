#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RSRunePillarAnimInstance.generated.h"

UCLASS()
class REMNANTSOUL_API URSRunePillarAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 각 본에 적용할 Yaw(도 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rune", meta=(PinShownByDefault))
	float TopYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rune", meta=(PinShownByDefault))
	float MidYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rune", meta=(PinShownByDefault))
	float BotYaw = 0.f;
};
