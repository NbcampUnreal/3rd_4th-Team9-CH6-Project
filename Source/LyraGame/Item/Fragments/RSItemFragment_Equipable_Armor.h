#pragma once

#include "RSDefine.h"
#include "RSItemFragment_Equipable.h"
#include "RSItemFragment_Equipable_Armor.generated.h"

UCLASS()
class URSItemFragment_Equipable_Armor : public URSItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	URSItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(URSItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="ArmorType == EArmorType::Chest", EditConditionHides))
	bool bIsFullBody = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
