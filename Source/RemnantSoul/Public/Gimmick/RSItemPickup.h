// Gimmick/RSItemPickup.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSItemPickup.generated.h"

// 기존 RSPotion.h에서 사용하던 인터페이스/부모(Interactable 등) 그대로 유지해야 함.
class USceneComponent;
class UStaticMeshComponent;
class URSItemData;

UCLASS()
class REMNANTSOUL_API ARSItemPickup : public AActor
{
	GENERATED_BODY()

public:
	ARSItemPickup();

	// 너 기존 헤더에 있던 인터페이스 함수 시그니처 그대로 유지
	virtual bool CanInteract_Implementation(AActor* Interactor) const;
	virtual void Interact_Implementation(AActor* Interactor);
	virtual URSItemData* GetItemData_Implementation() const;
	
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