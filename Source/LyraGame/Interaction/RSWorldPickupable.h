//#pragma once
//
//#include "RSPickupable.h"
//#include "RSWorldInteractable.h"
//#include "RSWorldPickupable.generated.h"
//
//UCLASS(Abstract, Blueprintable)
//class ARSWorldPickupable : public AActor, public IRSInteractable, public IRSPickupable
//{
//	GENERATED_BODY()
//	
//public:
//	ARSWorldPickupable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//public:
//	virtual FRSInteractionInfo GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const override { return InteractionInfo; }
//	virtual void SetPickupInfo(const FRSPickupInfo& InPickupInfo);
//	virtual FRSPickupInfo GetPickupInfo() const override { return PickupInfo; }
//
//protected:
//	UFUNCTION()
//	virtual void OnRep_PickupInfo();
//
//protected:
//	UPROPERTY(EditAnywhere, Category="Info")
//	FRSInteractionInfo InteractionInfo;
//	
//	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_PickupInfo, Category="Info")
//	FRSPickupInfo PickupInfo;
//};
