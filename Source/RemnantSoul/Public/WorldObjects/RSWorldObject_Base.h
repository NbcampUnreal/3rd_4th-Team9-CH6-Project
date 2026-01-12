#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSWorldObject_Base.generated.h"

UCLASS()
class REMNANTSOUL_API ARSWorldObject_Base : public AActor
{
	GENERATED_BODY()

public:
	ARSWorldObject_Base();

public:
	/* ASC 소유자(Avatar)를 WorldObject에 연결하는 최소 구현 */
	UFUNCTION(BlueprintCallable, Category = "RS|WorldObject")
	void SetOwnerFromASC(AActor* InOwnerFromASC);

	UFUNCTION(BlueprintCallable, Category = "RS|WorldObject")
	AActor* GetOwnerFromASC() const;

protected:
	virtual void BeginPlay() override;

	/* 필요 없으면 파생에서 Tick 끄는 것을 권장 */
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RS|WorldObject")
	TObjectPtr<AActor> OwnerFromASC;
};
