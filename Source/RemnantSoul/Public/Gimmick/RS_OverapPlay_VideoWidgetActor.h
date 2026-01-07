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
	
};
