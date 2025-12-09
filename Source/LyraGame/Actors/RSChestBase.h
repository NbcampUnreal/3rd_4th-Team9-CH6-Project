//#pragma once
//
//#include "RSDefine.h"
//#include "Interaction/RSWorldInteractable.h"
//#include "RSChestBase.generated.h"
//
//class URSItemTemplate;
//class UArrowComponent;
//class URSInventoryManagerComponent;
//
//UENUM(BlueprintType)
//enum class EItemAddType : uint8
//{
//	None,
//	Weapon,
//	Armor,
//	Custom
//};
//
//USTRUCT(BlueprintType)
//struct FItemAddRule
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	EItemAddType ItemAddType = EItemAddType::None;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="ItemAddType == EItemAddType::Custom", EditConditionHides))
//	TArray<TSubclassOf<URSItemTemplate>> CustomItemTemplateClasses;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	float ItemAddTypeRate = 0.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly)
//	TArray<EItemRarity> ItemRarities;
//};
//
//UENUM(BlueprintType)
//enum class EChestState : uint8
//{
//	Open,
//	Close
//};
//
//UCLASS()
//class ARSChestBase : public ARSWorldInteractable
//{
//	GENERATED_BODY()
//	
//public:
//	ARSChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//public:
//	virtual void BeginPlay() override;
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//public:
//	virtual FRSInteractionInfo GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const override;
//	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;
//	
//public:
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	void SetChestState(EChestState NewChestState);
//	
//private:
//	UFUNCTION()
//	void OnRep_ChestState();
//	
//protected:
//	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_ChestState)
//	EChestState ChestState = EChestState::Close;
//	
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	FRSInteractionInfo OpenedInteractionInfo;
//	
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	FRSInteractionInfo ClosedInteractionInfo;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	TObjectPtr<UAnimMontage> OpenMontage;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	TObjectPtr<UAnimMontage> CloseMontage;
//
//protected:
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	TArray<FItemAddRule> ItemAddRules;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info")
//	bool bShouldFallback = false;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info", meta=(EditCondition="bShouldFallback", EditConditionHides))
//	TSubclassOf<URSItemTemplate> FallbackItemTemplateClass;
//
//	UPROPERTY(EditDefaultsOnly, Category="Info", meta=(EditCondition="bShouldFallback", EditConditionHides))
//	EItemRarity FallbackItemItemRarity = EItemRarity::Poor;
//	
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<UArrowComponent> ArrowComponent;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<USkeletalMeshComponent> MeshComponent;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TObjectPtr<URSInventoryManagerComponent> InventoryManager;
//};
