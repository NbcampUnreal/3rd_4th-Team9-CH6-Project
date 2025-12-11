//#pragma once
//
//#include "Interaction/RSWorldInteractable.h"
//#include "RSStatueBase.generated.h"
//
//class UArrowComponent;
//class UNiagaraComponent;
//
//UCLASS()
//class ARSStatueBase : public ARSWorldInteractable
//{
//	GENERATED_BODY()
//	
//public:
//	ARSStatueBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//public:
//	virtual FRSInteractionInfo GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const override;
//	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
//	
//protected:
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	FRSInteractionInfo InteractionInfo;
//	
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UArrowComponent> ArrowComponent;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UStaticMeshComponent> MeshComponent;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
//	TObjectPtr<UNiagaraComponent> NiagaraComponent;
//};
