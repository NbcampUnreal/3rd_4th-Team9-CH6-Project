#pragma once

#include "CoreMinimal.h"
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_CombatStyle.generated.h"

class URSCombatStyleData;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_CombatStyle : public URSItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|Style")
	TObjectPtr<URSCombatStyleData> CombatStyle = nullptr;
};
