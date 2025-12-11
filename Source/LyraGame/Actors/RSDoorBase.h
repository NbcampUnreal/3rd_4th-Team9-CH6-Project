//#pragma once
//
//#include "Interaction/RSWorldInteractable.h"
//#include "RSDoorBase.generated.h"
//
//class UArrowComponent;
//
//UENUM(BlueprintType)
//enum class EDoorState : uint8
//{
//	Open_Forward,
//	Open_Backward,
//	Close
//};
//
//UCLASS()
//class ARSDoorBase : public ARSWorldInteractable
//{
//	GENERATED_BODY()
//	
//public:
//	ARSDoorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
//	
//public:
//	virtual FRSInteractionInfo GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const override;
//	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
//
//public:
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	void SetDoorState(EDoorState NewDoorState);
//
//protected:
//	UFUNCTION(BlueprintImplementableEvent)
//	void OnDoorStateChanged(EDoorState NewDoorState);
//	
//private:
//	UFUNCTION()
//	void OnRep_DoorState();
//
//public:
//	EDoorState GetDoorState() const { return DoorState; }
//	
//protected:
//	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_DoorState)
//	EDoorState DoorState = EDoorState::Close;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	FRSInteractionInfo OpenedInteractionInfo;
//	
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	FRSInteractionInfo ClosedInteractionInfo;
//	
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UArrowComponent> ArrowComponent;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UStaticMeshComponent> LeftMeshComponent;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UStaticMeshComponent> RightMeshComponent;
//};
