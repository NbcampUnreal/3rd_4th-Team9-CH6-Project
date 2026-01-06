// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick//PP_LightZone.h"

#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"

APP_LightZone::APP_LightZone()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(RootComponent);

	// "존 안에서만" 켜고 끌 거라서, 전역(Unbound) + BlendWeight 제어가 제일 안정적
	PostProcess->bUnbound = true;
	PostProcess->BlendWeight = 0.0f;
	CurrentPPBlend = 0.0f;

	// 오버랩 바인딩
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APP_LightZone::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APP_LightZone::OnTriggerEndOverlap);
}

void APP_LightZone::BeginPlay()
{
	Super::BeginPlay();

	ResolveDirectionalLight();

	bPlayerInside = false;
	CurrentPPBlend = 0.0f;
	PostProcess->BlendWeight = 0.0f;

	CurrentLightIntensity = LightIntensity_Off;

	if (TargetDirectionalLight)
	{
		TargetDirectionalLight->GetLightComponent()->SetIntensity(CurrentLightIntensity);

		if (bToggleVisibility)
		{
			TargetDirectionalLight->GetLightComponent()->SetVisibility(CurrentLightIntensity > KINDA_SMALL_NUMBER);
		}
	}
}

void APP_LightZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyPostProcess(DeltaSeconds);
	ApplyLight(DeltaSeconds);
}

bool APP_LightZone::IsPlayerActor(const AActor* Actor) const
{
	if (!Actor) return false;

	APawn* Pawn = Cast<APawn>(const_cast<AActor*>(Actor));
	if (!Pawn) return false;

	// 로컬/싱글 기준: 플레이어 0의 Pawn인지 확인
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	return (Pawn == PlayerPawn);
}

void APP_LightZone::ResolveDirectionalLight()
{
	if (TargetDirectionalLight) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(this, ADirectionalLight::StaticClass(), Found);

	for (AActor* A : Found)
	{
		if (A && (DirectionalLightTag.IsNone() || A->ActorHasTag(DirectionalLightTag)))
		{
			TargetDirectionalLight = Cast<ADirectionalLight>(A);
			break;
		}
	}

}

void APP_LightZone::ApplyPostProcess(float DeltaSeconds)
{
	const float TargetBlend = bPlayerInside ? 1.0f : 0.0f;
	CurrentPPBlend = FMath::FInterpTo(CurrentPPBlend, TargetBlend, DeltaSeconds, PPBlendSpeed);
	PostProcess->BlendWeight = CurrentPPBlend;
}

void APP_LightZone::ApplyLight(float DeltaSeconds)
{
	if (!TargetDirectionalLight) return;

	const float TargetIntensity = bPlayerInside ? LightIntensity_On : LightIntensity_Off;
	CurrentLightIntensity = FMath::FInterpTo(CurrentLightIntensity, TargetIntensity, DeltaSeconds, LightFadeSpeed);

	auto* LightComp = TargetDirectionalLight->GetLightComponent();
	if (!LightComp) return;

	LightComp->SetIntensity(CurrentLightIntensity);

	if (bToggleVisibility)
	{
		// 완전히 0 근처면 시각적으로/성능적으로 아예 꺼버림
		const bool bShouldBeVisible = (CurrentLightIntensity > 0.05f);
		LightComp->SetVisibility(bShouldBeVisible);
	}
}

void APP_LightZone::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (IsPlayerActor(OtherActor))
	{
		bPlayerInside = true;
	}
}

void APP_LightZone::OnTriggerEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (IsPlayerActor(OtherActor))
	{
		bPlayerInside = false;
	}
}
