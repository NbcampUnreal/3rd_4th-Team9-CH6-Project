#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AttributeSet.h"
#include "RSGameplayAbility_Sprint_Check.generated.h"

UCLASS()
class REMNANTSOUL_API URSGameplayAbility_Sprint_Check : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URSGameplayAbility_Sprint_Check();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	bool CheckSprintConditions(FVector& OutMoveVectorWS) const;
	void SendSprintActiveEvent(const FVector& MoveVectorWS) const;

	UFUNCTION(Server, Reliable)
	void Server_RequestSprint(const FVector& MoveVectorWS);

protected:
	// 전방 판정(로컬 방향 벡터 X가 이 값보다 커야 Sprint)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float ForwardThreshold = 0.25f;

	// (선택) 스태미나/자원 체크용. 없으면 무시하도록 구현
	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	FGameplayAttribute SprintCostAttribute;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Sprint")
	float MinRemainValueToSprint = 10.f;
};
