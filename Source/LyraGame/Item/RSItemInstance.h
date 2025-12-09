#pragma once

#include "RSDefine.h"
#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "System/RSGameplayTagStack.h"
#include "RSItemInstance.generated.h"

USTRUCT(BlueprintType)
struct FRSItemRarityProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditAnywhere)
	float Probability = 0;
};

UCLASS(BlueprintType)
class URSItemInstance : public UObject, public ILyraAbilitySourceInterface
{
	GENERATED_BODY()

public:
	URSItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;

public:
	void Init(int32 InItemTemplateID, EItemRarity InItemRarity);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(FGameplayTag StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag);

public:
	static EItemRarity DetermineItemRarity(const TArray<FRSItemRarityProbability>& ItemProbabilities);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetItemTemplateID() const { return ItemTemplateID; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EItemRarity GetItemRarity() const { return ItemRarity; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetStackCountByTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FRSGameplayTagStackContainer& GetStatContainer() const { return StatContainer; }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = "false", meta = (DeterminesOutputType = "FragmentClass"))
	const URSItemFragment* FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

private:
#if UE_WITH_IRIS
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

private:
	UPROPERTY(Replicated)
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY(Replicated)
	EItemRarity ItemRarity = EItemRarity::Poor;

	UPROPERTY(Replicated)
	FRSGameplayTagStackContainer StatContainer;
};
