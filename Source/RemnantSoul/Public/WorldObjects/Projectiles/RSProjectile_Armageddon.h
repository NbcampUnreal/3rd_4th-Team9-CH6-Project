#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/Projectiles/RSProjectile_Base.h"
#include "RSProjectile_Armageddon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class REMNANTSOUL_API ARSProjectile_Armageddon : public ARSProjectile_Base
{
	GENERATED_BODY()

public:
	ARSProjectile_Armageddon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Armageddon|VFX")
	TObjectPtr<UNiagaraComponent> TrailVFXComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Armageddon|VFX")
	TObjectPtr<UNiagaraSystem> TrailVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Armageddon|VFX")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Armageddon|Sound")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Armageddon|Damage")
	float Damage = 50.f;

protected:
	virtual void BeginPlay() override;
	virtual void HandleImpact(const FHitResult& Hit) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayImpactFX(const FVector_NetQuantize& Loc, const FRotator& Rot);

private:
	// FVector_NetQuantize -> FVector 변환을 안전하게 고정 (UE 버전/구현 차이 방지)
	static FVector ToVector(const FVector_NetQuantize& V);
};
