// Gimmick/RSItemPickup.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Interface/RSItemSource.h"
#include "RSItemPickup.generated.h"



UCLASS()
class REMNANTSOUL_API ARSItemPickup : public AActor,public IInteractable, public IRSItemSource
{
	GENERATED_BODY()

public:
	ARSItemPickup();

	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual URSItemData* GetItemData_Implementation() const override;
	
	void SetHighlight(bool bEnable);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Body;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<URSItemData> ItemData;
};