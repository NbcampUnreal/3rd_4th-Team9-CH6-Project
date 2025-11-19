#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CVWorldObject_Base.generated.h"

UCLASS()
class CHRONOVEIL_API ACVWorldObject_Base : public AActor
{
	GENERATED_BODY()
	
public:
	ACVWorldObject_Base();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|WorldObject")
	TObjectPtr<USceneComponent> SceneRoot;

	// 소유자 정보 (설치한 플레이어 / 발사한 플레이어)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|WorldObject")
	TWeakObjectPtr<AController> OwningController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CV|WorldObject")
	TWeakObjectPtr<APawn> OwningPawn;

public:
	void SetOwnerFromASC(AActor* InAvatar);

	FORCEINLINE AController* GetOwningController() const { return OwningController.Get(); }
	FORCEINLINE APawn* GetOwningPawn() const { return OwningPawn.Get(); }

};
