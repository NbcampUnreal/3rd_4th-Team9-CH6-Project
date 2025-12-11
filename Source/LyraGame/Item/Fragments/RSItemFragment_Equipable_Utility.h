#pragma once

#include "RSItemFragment_Equipable_Attachment.h"
#include "RSItemFragment_Equipable_Utility.generated.h"

class UGameplayEffect;

UCLASS()
class URSItemFragment_Equipable_Utility : public URSItemFragment_Equipable_Attachment
{
	GENERATED_BODY()

public:
	URSItemFragment_Equipable_Utility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(URSItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EUtilityType UtilityType = EUtilityType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> UtilityEffectClass;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatSet> RarityStatSets;
};
