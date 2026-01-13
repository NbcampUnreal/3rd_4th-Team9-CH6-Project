#include "Animation/AnimNotify/RSAnimNotify_EquipAction.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"

#include "GameFramework/Actor.h"
#include "Character/RSCharacter.h"

URSAnimNotify_EquipAction::URSAnimNotify_EquipAction()
{
}

FString URSAnimNotify_EquipAction::GetNotifyName_Implementation() const
{
	return TEXT("RS_EquipAction");
}

void URSAnimNotify_EquipAction::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp)
    {
        return;
    }

    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor)
    {
        return;
    }

    if (URSEquipmentManagerComponent* EqMgr = OwnerActor->FindComponentByClass<URSEquipmentManagerComponent>())
    {
        EqMgr->HandleEquipAnimAction(Action);
    }
}