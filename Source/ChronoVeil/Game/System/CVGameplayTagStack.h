#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CVGameplayTagStack.generated.h"

/**
 * 하나의 GameplayTag + StackCount 를 나타내는 구조체
 * 예: 탄약 개수처럼 "태그 + 개수" 형태로 관리할 때 사용
 */
USTRUCT(BlueprintType)
struct FCVGameplayTagStack
{
	GENERATED_BODY()

	FCVGameplayTagStack() {}
	FCVGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/**
 * CVGameplayTagStack 컨테이너
 * - 여러 GameplayTagStack을 관리
 * - LUT(TagToCountMap)를 사용해 조회 최적화
 */
USTRUCT(BlueprintType)
struct FCVGameplayTagStackContainer
{
	GENERATED_BODY()

	FCVGameplayTagStackContainer() {}

	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	bool HasTag(FGameplayTag Tag) const
	{
		return ContainsTag(Tag);
	}

	UPROPERTY()
	TArray<FCVGameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCountMap;
};
