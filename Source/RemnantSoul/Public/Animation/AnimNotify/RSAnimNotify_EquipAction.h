#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotify/RSAnimEquipAction.h"
#include "RSAnimNotify_EquipAction.generated.h"

UCLASS()
class REMNANTSOUL_API URSAnimNotify_EquipAction : public UAnimNotify
{
	GENERATED_BODY()

public:
	URSAnimNotify_EquipAction();

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

public:
	UPROPERTY(EditAnywhere, Category = "RS|Equip")
	ERSAnimEquipAction Action = ERSAnimEquipAction::AttachWeapon;
};
