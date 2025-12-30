#include "PlayerState/RSPlayerState.h"
#include "Engine/AssetManager.h"
#include "Character/RSHeroData.h"

ARSPlayerState::ARSPlayerState()
{
}

void ARSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[PS] BeginPlay: DefaultHeroDataAsset=%s"),
		*DefaultHeroDataAsset.ToSoftObjectPath().ToString());

	if (DefaultHeroDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[PS] DefaultHeroDataAsset is NULL. Set it in BP_RSPlayerState defaults."));
		LoadedHeroData = nullptr;
		return;
	}

	URSHeroData* HD = DefaultHeroDataAsset.LoadSynchronous();
	LoadedHeroData = HD;

	UE_LOG(LogTemp, Warning, TEXT("[PS] LoadedHeroData=%s"), *GetNameSafe(LoadedHeroData));

	if (LoadedHeroData && !bHeroDataReadyBroadcasted)
	{
		bHeroDataReadyBroadcasted = true;
		UE_LOG(LogTemp, Warning, TEXT("[PS] OnHeroDataReady Broadcast -> %s"), *GetNameSafe(LoadedHeroData));
		OnHeroDataReady.Broadcast(LoadedHeroData);
	}
}

void ARSPlayerState::SetHeroDataId_Single(const FPrimaryAssetId& InHeroDataId)
{
	UE_LOG(LogTemp, Warning, TEXT("[PS] SetHeroDataId_Single: %s"), *InHeroDataId.ToString());

	HeroDataId = InHeroDataId;
	bHeroDataReadyBroadcasted = false;
	ResolveHeroData();
}

void ARSPlayerState::ResolveHeroData()
{
	UE_LOG(LogTemp, Warning, TEXT("[PS] ResolveHeroData: HeroDataId=%s"), *HeroDataId.ToString());

	if (!HeroDataId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[PS] ResolveHeroData: Invalid HeroDataId"));
		LoadedHeroData = nullptr;
		return;
	}

	UAssetManager& AM = UAssetManager::Get();
	const FSoftObjectPath Path = AM.GetPrimaryAssetPath(HeroDataId);

	UE_LOG(LogTemp, Warning, TEXT("[PS] ResolveHeroData: Path=%s"), *Path.ToString());

	if (!Path.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[PS] PrimaryAssetPath INVALID. Check AssetManagerSettings PrimaryAssetTypes for HeroData."));
		LoadedHeroData = nullptr;
		return;
	}

	UObject* Obj = Path.TryLoad(); // 싱글이니 동기로 OK
	LoadedHeroData = Cast<URSHeroData>(Obj);

	UE_LOG(LogTemp, Warning, TEXT("[PS] ResolveHeroData: LoadedHeroData=%s"), *GetNameSafe(LoadedHeroData));

	if (LoadedHeroData && !bHeroDataReadyBroadcasted)
	{
		bHeroDataReadyBroadcasted = true;
		UE_LOG(LogTemp, Warning, TEXT("[PS] OnHeroDataReady Broadcast -> %s"), *GetNameSafe(LoadedHeroData));
		OnHeroDataReady.Broadcast(LoadedHeroData);
	}
}
