// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemDataAsset/RSItemData.h"
#include "RSPotion.generated.h"




UCLASS()
class REMNANTSOUL_API ARSPotion : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARSPotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USceneComponent> Root;
    
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMeshComponent> Body;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
   	TObjectPtr<URSItemData> ItemData;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
