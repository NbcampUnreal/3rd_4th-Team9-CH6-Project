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

	// 서버에서 설정
	void ServerSetHeroDataId(const FPrimaryAssetId& InHeroDataId);

	// 클라/서버 공통: 로드 완료된 HeroData 얻기
	const URSHeroData* GetHeroData() const { return LoadedHeroData; }

	FRSOnHeroDataReady OnHeroDataReady;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_HeroDataId();

	// Asset 로드
	void ResolveHeroData();

private:
	UPROPERTY(ReplicatedUsing = OnRep_HeroDataId)
	FPrimaryAssetId HeroDataId;

	UPROPERTY(Transient)
	TObjectPtr<const URSHeroData> LoadedHeroData = nullptr;
};
