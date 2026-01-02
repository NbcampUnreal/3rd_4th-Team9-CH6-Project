#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RSAnimNotify_RollEnd.generated.h"

UCLASS(meta = (DisplayName = "RS Roll End"))
class REMNANTSOUL_API URSAnimNotify_RollEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	URSAnimNotify_RollEnd();

#if ENGINE_MAJOR_VERSION >= 5
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
#endif

	// 호환용(에디터/버전차 대비)
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	void SendRollEndEvent(USkeletalMeshComponent* MeshComp) const;
};
