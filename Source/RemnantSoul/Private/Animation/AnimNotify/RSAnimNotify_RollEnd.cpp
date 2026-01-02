#include "Animation/AnimNotify/RSAnimNotify_RollEnd.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSAnimNotify_RollEnd)

URSAnimNotify_RollEnd::URSAnimNotify_RollEnd()
{
	// 필요하면 여기서 색상/카테고리 같은 거 커스터마이즈 가능
}

#if ENGINE_MAJOR_VERSION >= 5
void URSAnimNotify_RollEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	SendRollEndEvent(MeshComp);
}
#endif

void URSAnimNotify_RollEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	SendRollEndEvent(MeshComp);
}

void URSAnimNotify_RollEnd::SendRollEndEvent(USkeletalMeshComponent* MeshComp) const
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

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	// Payload는 비워도 되지만, EventTag를 같이 넣어두면 디버그에 도움됨
	FGameplayEventData Payload;
	Payload.EventTag = Tags.Event_Roll_End;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, Tags.Event_Roll_End, Payload);
}
