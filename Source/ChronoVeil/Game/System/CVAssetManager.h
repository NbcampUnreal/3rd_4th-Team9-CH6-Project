#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CVAssetManager.generated.h"

UCLASS()
class CHRONOVEIL_API UCVAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UCVAssetManager();

	static UCVAssetManager& Get();

	// UAssetManager
	virtual void StartInitialLoading() override;

	/** 커맨드라인 -LogAssetLoads 여부 */
	static bool ShouldLogAssetLoads();

	/** 동기 로딩 래퍼 */
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	/** 오브젝트 로딩 + (선택) 메모리에 유지 */
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

	/** 클래스 로딩 + (선택) 메모리에 유지 */
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& ClassPtr, bool bKeepInMemory = true);

	/** GC 보호용 캐시 */
	void AddLoadedAsset(const UObject* Asset);

	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection SyncObject;
};

template<typename AssetType>
AssetType* UCVAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	AssetType* Loaded = nullptr;
	const FSoftObjectPath& Path = AssetPtr.ToSoftObjectPath();
	if (Path.IsValid())
	{
		Loaded = AssetPtr.Get();
		if (!Loaded)
		{
			Loaded = Cast<AssetType>(SynchronousLoadAsset(Path));
			ensureAlwaysMsgf(Loaded, TEXT("Failed to load asset [%s]"), *AssetPtr.ToString());
		}
		if (Loaded && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(Loaded));
		}
	}
	return Loaded;
}

template<typename AssetType>
TSubclassOf<AssetType> UCVAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& ClassPtr, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedClass;
	const FSoftObjectPath& Path = ClassPtr.ToSoftObjectPath();
	if (Path.IsValid())
	{
		LoadedClass = ClassPtr.Get();
		if (!LoadedClass)
		{
			LoadedClass = Cast<UClass>(SynchronousLoadAsset(Path));
			ensureAlwaysMsgf(LoadedClass, TEXT("Failed to load asset class [%s]"), *ClassPtr.ToString());
		}
		if (LoadedClass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedClass));
		}
	}
	return LoadedClass;
}