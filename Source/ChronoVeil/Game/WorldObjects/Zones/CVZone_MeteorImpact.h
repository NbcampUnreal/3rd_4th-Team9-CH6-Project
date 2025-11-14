// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/WorldObjects/Zones/CVZone_Base.h"
#include "CVZone_MeteorImpact.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class CHRONOVEIL_API ACVZone_MeteorImpact : public ACVZone_Base
{
	GENERATED_BODY()
	
public:
    ACVZone_MeteorImpact();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* MeteorVFX;

    // 하늘에서 떨어지는 Meteor VFX
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
    UNiagaraSystem* FallingVFX;

    // 땅에 부딪혔을 때 폭발 및 마그마 VFX
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
    UNiagaraSystem* ImpactVFX;

    // 메테오 낙하 후 충돌까지 지연 시간
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
    float ImpactDelay = 5.0f;

    // 피해량
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
    float Damage = 50.f;

    FTimerHandle ImpactTimer;


protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnImpact();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayImpactVFX();
};
