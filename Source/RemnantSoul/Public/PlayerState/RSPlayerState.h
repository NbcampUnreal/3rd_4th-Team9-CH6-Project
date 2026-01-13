#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UObject/PrimaryAssetId.h"
#include "RSPlayerState.generated.h"

class URSHeroData;

DECLARE_MULTICAST_DELEGATE_OneParam(FRSOnHeroDataReady, const URSHeroData*);

UCLASS()
class REMNANTSOUL_API ARSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ARSPlayerState();

	// 싱글: 이 값만 세팅하면 됨 (BP/디폴트에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "RS|Hero")
	FPrimaryAssetId DefaultHeroDataId;

	// 로드 완료된 HeroData 얻기
	const URSHeroData* GetHeroData() const { return LoadedHeroData; }

	FRSOnHeroDataReady OnHeroDataReady;

protected:
	virtual void BeginPlay() override;

private:
	// 싱글: 굳이 Replicated 필요 없음
	FPrimaryAssetId HeroDataId;

	UPROPERTY(EditDefaultsOnly, Category = "RS|Hero")
	TSoftObjectPtr<URSHeroData> DefaultHeroDataAsset;
	
	UPROPERTY(Transient)
	TObjectPtr<const URSHeroData> LoadedHeroData = nullptr;

	bool bHeroDataReadyBroadcasted = false;

	void SetHeroDataId_Single(const FPrimaryAssetId& InHeroDataId);
	void ResolveHeroData();
};
