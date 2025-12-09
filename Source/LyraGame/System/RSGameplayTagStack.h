#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "RSGameplayTagStack.generated.h"

struct FRSGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

USTRUCT(BlueprintType)
struct FRSGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FRSGameplayTagStack() { }

	FRSGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount) { }

public:
	const FGameplayTag& GetStackTag() const { return Tag; }
	int32 GetStackCount() const { return StackCount; }
	
	FString GetDebugString() const;
	
private:
	friend FRSGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FRSGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FRSGameplayTagStackContainer() { }

public:
	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag);

public:
	const TArray<FRSGameplayTagStack>& GetStacks() const { return Stacks; }
	
	int32 GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }
	bool ContainsTag(FGameplayTag Tag) const { return TagToCountMap.Contains(Tag); }
	
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRSGameplayTagStack, FRSGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FRSGameplayTagStack> Stacks;

	UPROPERTY(NotReplicated)
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FRSGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FRSGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
