#include "RSItemInstance.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS

#include "RSItemTemplate.h"
#include "LyraLogChannels.h"
#include "Data/RSItemData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemInstance)

URSItemInstance::URSItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void URSItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemTemplateID);
	DOREPLIFETIME(ThisClass, ItemRarity);
	DOREPLIFETIME(ThisClass, StatContainer);
}

float URSItemInstance::GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 0;
}

float URSItemInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 0;
}

void URSItemInstance::Init(int32 InItemTemplateID, EItemRarity InItemRarity)
{
	if (InItemTemplateID <= INDEX_NONE || InItemRarity == EItemRarity::Count)
		return;

	ItemTemplateID = InItemTemplateID;
	ItemRarity = InItemRarity;

	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemTemplateID);
	for (const URSItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

#if UE_WITH_IRIS
void URSItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void URSItemInstance::AddStatTagStack(FGameplayTag StatTag, int32 StackCount)
{
	StatContainer.AddStack(StatTag, StackCount);
}

void URSItemInstance::RemoveStatTagStack(FGameplayTag StatTag)
{
	StatContainer.RemoveStack(StatTag);
}

EItemRarity URSItemInstance::DetermineItemRarity(const TArray<FRSItemRarityProbability>& ItemProbabilities)
{
	float TotalProbability = 0.f;
	for (const FRSItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		TotalProbability += ItemProbability.Probability;
	}

	if (TotalProbability > 100.f)
		return EItemRarity::Count;

	float SumProbability = 0.f;
	float RandomValue = FMath::RandRange(0.f, 100.f);

	for (const FRSItemRarityProbability& ItemProbability : ItemProbabilities)
	{
		SumProbability += ItemProbability.Probability;
		if (RandomValue < SumProbability)
		{
			return ItemProbability.Rarity;
		}
	}

	return EItemRarity::Count;
}

bool URSItemInstance::HasStatTag(FGameplayTag StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

int32 URSItemInstance::GetStackCountByTag(FGameplayTag StatTag) const
{
	return StatContainer.GetStackCount(StatTag);
}

const URSItemFragment* URSItemInstance::FindFragmentByClass(TSubclassOf<URSItemFragment> FragmentClass) const
{
	if (ItemTemplateID > INDEX_NONE && FragmentClass)
	{
		const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemTemplateID);
		return ItemTemplate.FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
