//#include "RSWorldInteractable.h"
//
//#include "RSGameplayTags.h"
//#include "Character/LyraCharacter.h"
//#include "Net/UnrealNetwork.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSWorldInteractable)
//
//ARSWorldInteractable::ARSWorldInteractable(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	bReplicates = true;
//}
//
//void ARSWorldInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, bIsUsed);
//}
//
//void ARSWorldInteractable::OnInteractActiveStarted(AActor* Interactor)
//{
//	if (IsValid(Interactor) == false)
//		return;
//	
//	if (HasAuthority())
//	{
//		CachedInteractors.Add(Interactor);
//	}
//
//	K2_OnInteractActiveStarted(Interactor);
//}
//
//void ARSWorldInteractable::OnInteractActiveEnded(AActor* Interactor)
//{
//	if (IsValid(Interactor) == false)
//		return;
//	
//	if (HasAuthority())
//	{
//		CachedInteractors.Remove(Interactor);
//	}
//
//	K2_OnInteractActiveEnded(Interactor);
//}
//
//void ARSWorldInteractable::OnInteractionSuccess(AActor* Interactor)
//{
//	if (IsValid(Interactor) == false)
//		return;
//	
//	if (HasAuthority())
//	{
//		if (bCanUsed)
//		{
//			bIsUsed = true;
//		}
//
//		for (TWeakObjectPtr<AActor> CachedInteractor : CachedInteractors)
//		{
//			if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CachedInteractor.Get()))
//			{
//				if (Interactor == LyraCharacter)
//					continue;
//				
//				if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
//				{
//					FGameplayTagContainer CancelAbilitiesTag;
//					CancelAbilitiesTag.AddTag(RSGameplayTags::Ability_Interact_Active);
//					ASC->CancelAbilities(&CancelAbilitiesTag);
//				}
//			}
//		}
//		
//		CachedInteractors.Empty();
//	}
//	
//	K2_OnInteractionSuccess(Interactor);
//}
//
//bool ARSWorldInteractable::CanInteraction(const FRSInteractionQuery& InteractionQuery) const
//{
//	return bCanUsed ? (bIsUsed == false) : true;
//}
