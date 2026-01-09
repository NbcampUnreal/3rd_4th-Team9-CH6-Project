// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RS_OverapPlay_VideoWidgetActor.generated.h"


class UBoxComponent;
class UUserWidget;

UCLASS()
class REMNANTSOUL_API ARS_OverapPlay_VideoWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARS_OverapPlay_VideoWidgetActor();

protected:
	virtual void BeginPlay() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overlap")
	UBoxComponent* OverlapBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	int32 ZOrder = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Overlap")
	bool bOneShot = true;

private:
	bool bTriggered = false;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void HandleWidgetSpawn(AActor* TargetActor);

	//오버랩 시 플레이어 캐릭터를 이동시키기 위한 파트로 기간 내 적당한 개발을 진행하기 버거워 임시 조치임.
	//가장 베스트인 점은 Ability를 하나 추가해 이벤트 태그를 보내고 그 이벤트 태그를 받을 시 컷신 실행 및 보스룸 이동을 담당하는 편임.
	//하지만 Ability를 추가 개발할 시간적 여유가 부족해 대안으로 오버랩 액터에서 직접 캐릭터를 이동시키는 대안책으로 진행.
	//오버랩 시 컷신이 실행되고 보스룸 내부로 이동된 직후 이 액터는 Destroy 되므로 플레이어 무브먼트를 일시 중지 할 수 없음.
	UPROPERTY(EditAnywhere, Category = "BossRoom")
	bool bTeleportPlayer = true;

	UPROPERTY(EditAnywhere, Category = "BossRoom", meta = (EditCondition="bTeleportPlayer"))
	FTransform BossRoomTransform;
	
	void TeleportActorToBossRoom(AActor* TargetActor);

};
