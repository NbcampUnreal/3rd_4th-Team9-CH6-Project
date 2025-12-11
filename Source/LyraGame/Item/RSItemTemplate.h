#pragma once

#include "RSItemTemplate.generated.h"

class URSItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class URSItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(URSItemInstance* Instance) const {}
};

UCLASS(Blueprintable, Const, Abstract)
class URSItemTemplate : public UObject
{
	GENERATED_BODY()

public:
	URSItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = "false", meta = (DeterminesOutputType = "FragmentClass"))
	const URSItemFragment* FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint SlotCount = FIntPoint::ZeroValue;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> PickupableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<URSItemFragment>> Fragments;
};
