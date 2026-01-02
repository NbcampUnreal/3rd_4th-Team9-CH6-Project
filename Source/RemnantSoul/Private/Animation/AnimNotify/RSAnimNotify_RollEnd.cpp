#include "Animation/AnimNotify/RSAnimNotify_RollEnd.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSAnimNotify_RollEnd)

URSAnimNotify_RollEnd::URSAnimNotify_RollEnd()
{
}

#if ENGINE_MAJOR_VERSION >= 5
void URSAnimNotify_RollEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	SendRollEndEvent(MeshComp, Animation);
}
#endif

void URSAnimNotify_RollEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	SendRollEndEvent(MeshComp, Animation);
}

void URSAnimNotify_RollEnd::SendRollEndEvent(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[RollEndNotify] Fired Owner=%s Anim=%s"),
		*GetNameSafe(OwnerActor),
		*GetNameSafe(Animation));

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	FGameplayEventData Payload;
	Payload.EventTag = Tags.Event_Roll_End;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, Tags.Event_Roll_End, Payload);
}
