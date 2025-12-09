//#include "RSAnimNotify_WeaponNiagaraEffect.h"
//
//#include "Actors/RSEquipmentBase.h"
//#include "Character/LyraCharacter.h"
//#include "Item/Managers/RSEquipManagerComponent.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSAnimNotify_WeaponNiagaraEffect)
//
//URSAnimNotify_WeaponNiagaraEffect::URSAnimNotify_WeaponNiagaraEffect()
//	: Super()
//{
//#if WITH_EDITORONLY_DATA
//	bShouldFireInEditor = false;
//#endif
//}
//
//void URSAnimNotify_WeaponNiagaraEffect::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
//{
//	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
//	Super::Notify(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, EventReference);
//}
//
//USkeletalMeshComponent* URSAnimNotify_WeaponNiagaraEffect::GetWeaponMeshComponent(USkeletalMeshComponent* MeshComponent) const
//{
//	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
//
//	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
//	{
//		if (URSEquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<URSEquipManagerComponent>())
//		{
//			if (ARSEquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
//			{
//				WeaponMeshComponent = WeaponActor->MeshComponent;
//			}
//		}
//	}
//
//	return WeaponMeshComponent;
//}
