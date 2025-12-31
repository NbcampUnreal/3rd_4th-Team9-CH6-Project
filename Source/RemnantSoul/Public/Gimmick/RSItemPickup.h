// Gimmick/RSItemPickup.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "RSItemPickup.generated.h"



UCLASS()
class REMNANTSOUL_API ARSItemPickup : public AActor,public IInteractable
{
	GENERATED_BODY()

public:
	ARSItemPickup();

	// 너 기존 헤더에 있던 인터페이스 함수 시그니처 그대로 유지
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual URSItemData* GetItemData_Implementation() const override;
	
	void SetHighlight(bool bEnable);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Body;

	// 월드에 놓인 아이템 데이터 (에디터 지정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<URSItemData> ItemData;
};