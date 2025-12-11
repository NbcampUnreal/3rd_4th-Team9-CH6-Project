#pragma once

#include "RSDefine.h"
#include "RSItemFragment_Equipable_Attachment.h"
#include "RSItemFragment_Equipable_Weapon.generated.h"

UCLASS()
class URSItemFragment_Equipable_Weapon : public URSItemFragment_Equipable_Attachment
{
	GENERATED_BODY()
	
public:
	URSItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	virtual void OnInstanceCreated(URSItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> AttackSwingSound;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<const ULyraAbilitySet>> SkillAbilitySets;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
