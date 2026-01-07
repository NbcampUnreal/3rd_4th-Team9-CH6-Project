// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RS_OverapPlay_VideoWidgetActor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

ARS_OverapPlay_VideoWidgetActor::ARS_OverapPlay_VideoWidgetActor()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	SetRootComponent(OverlapBox);

	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapBox->SetGenerateOverlapEvents(true);
}

void ARS_OverapPlay_VideoWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ARS_OverapPlay_VideoWidgetActor::OnOverlapBegin
	);
}

void ARS_OverapPlay_VideoWidgetActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || bTriggered)
	{
		return;
	}

	HandleWidgetSpawn(OtherActor);

	if (bOneShot)
	{
		bTriggered = true;
		Destroy();
	}
}

void ARS_OverapPlay_VideoWidgetActor::HandleWidgetSpawn(AActor* TargetActor)
{
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OverlapUI] WidgetClass is null"));
		return;
	}

	APawn* Pawn = Cast<APawn>(TargetActor);
	if (!Pawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC)
	{
		return;
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
	if (!Widget)
	{
		return;
	}

	Widget->AddToViewport(ZOrder);

	UE_LOG(LogTemp, Log, TEXT("[OverlapUI] Widget spawned and added to viewport"));
}