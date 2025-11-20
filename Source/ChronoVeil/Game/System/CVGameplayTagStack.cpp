#include "Game/System/CVGameplayTagStack.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CVGameplayTagStack)

void FCVGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		for (FCVGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				return;
			}
		}

		FCVGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
		TagToCountMap.Add(Tag, NewStack.StackCount);
	}
}

void FCVGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		// Iterator 사용 : 순회 중 원소 삭제에 유리
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FCVGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				// 0 이하로 떨어지면 아예 제거
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
				}
				return;
			}
		}
	}
}
