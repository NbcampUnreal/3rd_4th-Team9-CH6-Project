// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemDataAsset/RSItemData.h"
#include "Interface/Interactable.h"
#include "RSPotion.generated.h"




UCLASS()
class REMNANTSOUL_API ARSPotion : public AActor, public IInteractable
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARSPotion();
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual URSItemData* GetItemData_Implementation() const override;
protected:
	
	
	
	UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USceneComponent> Root;
    
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMeshComponent> Body;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
   	TObjectPtr<URSItemData> ItemData;
public:	
	

};
