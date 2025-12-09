//#pragma once
//
//#include "Interaction/RSWorldPickupable.h"
//#include "RSPickupableItemBase.generated.h"
//
//class UBoxComponent;
//class UProjectileMovementComponent;
//
//UCLASS()
//class ARSPickupableItemBase : public ARSWorldPickupable
//{
//	GENERATED_BODY()
//	
//public:
//	ARSPickupableItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void OnRep_PickupInfo() override;
//	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
//
//protected:
//	UPROPERTY(EditDefaultsOnly)
//	bool bAutoCollisionResize = true;
//	
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UBoxComponent> MovementCollision;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UBoxComponent> PickupCollision;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UStaticMeshComponent> MeshComponent;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
//
//protected:
//	UPROPERTY(EditDefaultsOnly)
//	FVector2D MaxMovementCollisionExtent = FVector2D(16.f, 16.f);
//
//	UPROPERTY(EditDefaultsOnly)
//	FVector2D MinPickupCollisionExtent = FVector2D(32.f, 32.f);
//};
