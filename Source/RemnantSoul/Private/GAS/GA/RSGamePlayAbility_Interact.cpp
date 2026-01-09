#include "GAS/GA/RSGamePlayAbility_Interact.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "Interface/Interactable.h"
#include "RSGameplayTags.h"


URSGamePlayAbility_Interact::URSGamePlayAbility_Interact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags = GetAssetTags(); 
	Tags.AddTag(FRSGameplayTags::Get().Ability_Interact);
	SetAssetTags(Tags);
}

void URSGamePlayAbility_Interact::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FHitResult Hit;
	if (!DoInteractTrace(ActorInfo, Hit))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* HitActor = Hit.GetActor();
	AActor* Interactor = ActorInfo->AvatarActor.Get();

	if (IsValid(HitActor) && HitActor->Implements<UInteractable>())
	{
		const bool bCan = IInteractable::Execute_CanInteract(HitActor, Interactor);
		if (bCan)
		{
			IInteractable::Execute_Interact(HitActor, Interactor);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool URSGamePlayAbility_Interact::DoInteractTrace(
	const FGameplayAbilityActorInfo* ActorInfo,
	FHitResult& OutHit
) const
{
	 const AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
    if (!IsValid(Avatar))
    {
        return false;
    }

    APlayerController* PC = ActorInfo->PlayerController.Get();
    if (!PC)
    {
        PC = Cast<APlayerController>(Avatar->GetInstigatorController());
    }
    if (!PC || !PC->PlayerCameraManager)
    {
        return false;
    }

    const FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
    const FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();
    const FVector Start = CamLoc;
    const FVector End   = Start + (CamRot.Vector() * TraceDistance);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(RSGA_InteractTrace), bTraceComplex);
    Params.AddIgnoredActor(Avatar);

    const UWorld* World = Avatar->GetWorld();
    if (!World)
    {
        return false;
    }

    const bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        TraceChannel,
        Params
    );



    return bHit;
}
