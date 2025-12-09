//#pragma once
//
//#include "AbilitySystemBlueprintLibrary.h"
//#include "RSInteractionInfo.h"
//#include "RSInteractionQuery.h"
//#include "AbilitySystem/Attributes/RSCombatSet.h"
//#include "UObject/Interface.h"
//#include "RSInteractable.generated.h"
//
//class IRSInteractable;
//struct FRSInteractionInfo;
//struct FRSInteractionQuery;
//
//class FRSInteractionInfoBuilder
//{
//public:
//	FRSInteractionInfoBuilder(TScriptInterface<IRSInteractable> InInteractable, TArray<FRSInteractionInfo>& InInteractionInfos)
//		: Interactable(InInteractable)
//		, InteractionInfos(InInteractionInfos) { }
//
//public:
//	void AddInteractionInfo(const FRSInteractionInfo& InteractionInfo)
//	{
//		FRSInteractionInfo& Entry = InteractionInfos.Add_GetRef(InteractionInfo);
//		Entry.Interactable = Interactable;
//	}
//	
//private:
//	TScriptInterface<IRSInteractable> Interactable;
//	TArray<FRSInteractionInfo>& InteractionInfos;
//};
//
//UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
//class URSInteractable : public UInterface
//{
//	GENERATED_BODY()
//};
//
//class IRSInteractable
//{
//	GENERATED_BODY()
//
//public:
//	virtual FRSInteractionInfo GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const { return FRSInteractionInfo(); }
//	
//	virtual void GatherPostInteractionInfos(const FRSInteractionQuery& InteractionQuery, FRSInteractionInfoBuilder& InteractionInfoBuilder) const
//	{
//		FRSInteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);
//	
//		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractionQuery.RequestingAvatar.Get()))
//		{
//			float Resourcefulness = AbilitySystem->GetNumericAttribute(URSCombatSet::GetResourcefulnessAttribute());
//			InteractionInfo.Duration = FMath::Max<float>(0.f, InteractionInfo.Duration - Resourcefulness * 0.01f);
//		}
//	
//		InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
//	}
//	
//	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const { }
//	
//	UFUNCTION(BlueprintCallable)
//	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const { }
//	
//	UFUNCTION(BlueprintCallable)
//	virtual bool CanInteraction(const FRSInteractionQuery& InteractionQuery) const { return true; }
//};
