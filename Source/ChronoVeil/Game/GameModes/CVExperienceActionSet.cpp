//#pragma once
//
//#include "CoreMinimal.h"
//#include "UObject/Object.h"
//#include "CVExperienceAction.generated.h"
//
//class UWorld;
//
///**
// * Executable unit run by ExperienceManager (server and/or client).
// */
//UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
//class CHRONOVEIL_API UCVExperienceAction : public UObject
//{
//	GENERATED_BODY()
//
//public:
//	/** Called when action object is created/registered. Lightweight. */
//	UFUNCTION(BlueprintNativeEvent, Category = "Experience")
//	void OnRegister();
//
//	/** Called when experience activates (server and/or client). */
//	UFUNCTION(BlueprintNativeEvent, Category = "Experience")
//	void OnExecute(UWorld* World);
//
//	/** Called when experience deactivates / map teardown. */
//	UFUNCTION(BlueprintNativeEvent, Category = "Experience")
//	void OnTeardown(UWorld* World);
//};
//
//		// Avoid double-finish from user code.
//		return;
//	}
//
//	// Fire delegate before clearing caches so BP listeners can still query.
//	OnFinished.Broadcast(this);
//
//	EndInternal();
//}
