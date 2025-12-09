#pragma once

#include "RSItemData.generated.h"

class URSItemTemplate;

UCLASS(BlueprintType, Const, meta=(DisplayName="RS Item Data"))
class URSItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const URSItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	const URSItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<URSItemTemplate> ItemTemplateClass) const;
	
	void GetAllItemTemplateClasses(TArray<TSubclassOf<URSItemTemplate>>& OutItemTemplateClasses) const;
	const TArray<TSubclassOf<URSItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	const TArray<TSubclassOf<URSItemTemplate>>& GetArmorItemTemplateClasses() const { return ArmorItemTemplateClasses; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<URSItemTemplate>> ItemTemplateIDToClass;

	UPROPERTY()
	TMap<TSubclassOf<URSItemTemplate>, int32> ItemTemplateClassToID;

private:
	UPROPERTY()
	TArray<TSubclassOf<URSItemTemplate>> WeaponItemTemplateClasses;

	UPROPERTY()
	TArray<TSubclassOf<URSItemTemplate>> ArmorItemTemplateClasses;
};
