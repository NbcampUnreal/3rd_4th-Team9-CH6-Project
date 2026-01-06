#include "Input/RSInputConfig.h"
#include "InputAction.h"
#include "RemnantSoul.h"

const UInputAction* URSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	EnsureCachesBuilt();

	if (const TObjectPtr<const UInputAction>* Found = NativeActionMap.Find(InputTag))
	{
		return Found->Get();
	}

	if (bLogNotFound)
	{
		UE_LOG(LogRemnantSoul, Warning, TEXT("[InputConfig] Native InputAction not found for Tag: %s (%s)"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}

const UInputAction* URSInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	EnsureCachesBuilt();

	if (const TObjectPtr<const UInputAction>* Found = AbilityActionMap.Find(InputTag))
	{
		return Found->Get();
	}

	if (bLogNotFound)
	{
		UE_LOG(LogRemnantSoul, Warning, TEXT("[InputConfig] Ability InputAction not found for Tag: %s (%s)"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}

void URSInputConfig::EnsureCachesBuilt() const
{
	if (bCacheBuilt)
	{
		return;
	}

	RebuildCaches();
	bCacheBuilt = true;
}

void URSInputConfig::RebuildCaches() const
{
	NativeActionMap.Reset();
	AbilityActionMap.Reset();

	auto BuildMap =
		[this](const TArray<FRSInputAction>& Src, TMap<FGameplayTag, TObjectPtr<const UInputAction>>& Dst, const TCHAR* Label)
		{
			for (const FRSInputAction& A : Src)
			{
				if (!A.InputTag.IsValid())
				{
					UE_LOG(LogRemnantSoul, Warning, TEXT("[InputConfig] %s has invalid InputTag (%s)"), Label, *GetNameSafe(this));
					continue;
				}

				if (!A.InputAction)
				{
					UE_LOG(LogRemnantSoul, Warning, TEXT("[InputConfig] %s Tag=%s has null InputAction (%s)"),
						Label, *A.InputTag.ToString(), *GetNameSafe(this));
					continue;
				}

				if (Dst.Contains(A.InputTag))
				{
					// 이건 강하게 잡는 게 맞다(데이터가 잘못된 상태)
					UE_LOG(LogRemnantSoul, Error, TEXT("[InputConfig] %s duplicate InputTag: %s (%s)"),
						Label, *A.InputTag.ToString(), *GetNameSafe(this));
					continue;
				}

				Dst.Add(A.InputTag, A.InputAction);
			}
		};

	BuildMap(NativeInputActions, NativeActionMap, TEXT("NativeInputActions"));
	BuildMap(AbilityInputActions, AbilityActionMap, TEXT("AbilityInputActions"));
}

#if WITH_EDITOR
void URSInputConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 어떤 프로퍼티가 바뀌든 캐시는 다시 빌드되도록
	bCacheBuilt = false;
}
#endif
