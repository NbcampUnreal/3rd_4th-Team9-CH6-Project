#include "PlayerState/RSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/AssetManager.h"
#include "Character/RSHeroData.h"

ARSPlayerState::ARSPlayerState()
{
}

void ARSPlayerState::ServerSetHeroDataId(const FPrimaryAssetId& InHeroDataId)
{
	if (!HasAuthority()) return;
	HeroDataId = InHeroDataId;
	ResolveHeroData(); // 서버도 즉시 로드
}

void ARSPlayerState::OnRep_HeroDataId()
{
	ResolveHeroData(); // 클라도 로드
}

void ARSPlayerState::ResolveHeroData()
{
	if (!HeroDataId.IsValid())
	{
		LoadedHeroData = nullptr;
		return;
	}

	UAssetManager& AM = UAssetManager::Get();
	const FSoftObjectPath Path = AM.GetPrimaryAssetPath(HeroDataId);
	if (!Path.IsValid())
	{
		LoadedHeroData = nullptr;
		return;
	}

	UObject* Obj = Path.TryLoad();
	LoadedHeroData = Cast<URSHeroData>(Obj);

	if (LoadedHeroData)
	{
		OnHeroDataReady.Broadcast(LoadedHeroData);
	}
}

void ARSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARSPlayerState, HeroDataId);
}
