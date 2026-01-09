// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RS_OverapPlay_VideoWidgetActor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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

	TeleportActorToBossRoom(TargetActor);

	UE_LOG(LogTemp, Log, TEXT("[OverlapUI] Widget spawned and added to viewport"));

}

void ARS_OverapPlay_VideoWidgetActor::TeleportActorToBossRoom(AActor* TargetActor)
{
	if (!bTeleportPlayer || !TargetActor)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(TargetActor);
	if (!Character)
	{
		return;
	}

	
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	MoveComp->StopMovementImmediately();

	FVector TargetLocation = BossRoomTransform.GetLocation();
	FRotator TargetRotation = BossRoomTransform.GetRotation().Rotator();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	//입력한 좌표의 Z값에서 200을 더한 값과 500을 뺀 값을 통해 트레이스의 시작점과 중단점을 계산
	const FVector TraceStart = TargetLocation + FVector(0.f, 0.f, HalfHeight + 200.f);
	const FVector TraceEnd   = TargetLocation - FVector(0.f, 0.f, HalfHeight + 500.f);

	//라인트레이스를 통해 바닥에 파묻히거나 허공에서 허우적거리지 않도록 보정.
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,TraceStart,TraceEnd,ECC_Visibility,Params);

	if (bHit)
	{
		TargetLocation.Z = Hit.ImpactPoint.Z + HalfHeight;
	}

	//라인트레이스로 보정받은 최종 지면 좌표
	Character->SetActorLocationAndRotation(TargetLocation,TargetRotation,false,nullptr,ETeleportType::TeleportPhysics);

	//만일 캐릭터가 추락 상태로 진입하고 바닥에 닿았으나 Walk로 돌아오지 못하는 케이스를 방지
	MoveComp->SetMovementMode(MOVE_Walking);

	Destroy();
}