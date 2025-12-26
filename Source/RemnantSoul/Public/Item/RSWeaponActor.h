#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSWeaponActor.generated.h"

class USkeletalMeshComponent;

UCLASS(Blueprintable)
class REMNANTSOUL_API ARSWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	ARSWeaponActor();

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh;
};
