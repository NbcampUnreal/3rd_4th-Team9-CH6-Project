//// CVPlayerState.h
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/PlayerState.h"
//#include "AbilitySystemInterface.h"
//#include "CVPlayerState.generated.h"
//
//// ── Forward Declarations (포인터/레퍼런스만 쓰므로 전방선언 OK)
//class UAbilitySystemComponent;
//class UCVAbilitySystemComponent;
//class UCVPawnData;
//class UCVExperienceDefinition;
//
///**
// * ChronoVeil 전용 PlayerState
// * - ASC 보관(서버 소유), PawnData 보관(Replicate)
// * - 경험 로딩 완료 시 PawnData 확정 및 Ability 부여
// */
//UCLASS()
//class CHRONOVEIL_API ACVPlayerState : public APlayerState, public IAbilitySystemInterface
//{
//	GENERATED_BODY()
//
//public:
//	ACVPlayerState();
//
//	// ───── PawnData Access ─────
//	template <class T> const T* GetPawnData() const { return Cast<T>(PawnData); }
//
//	/** Raw PawnData 읽기 (블루프린트 노출 원하면 BlueprintCallable 지정) */
//	UFUNCTION(BlueprintCallable, Category = "CV|PlayerState")
//	const UCVPawnData* GetPawnDataRaw() const { return PawnData; }
//
//	/** 경험 로딩 완료 콜백(서버에서 PawnData 확정) */
//	UFUNCTION()
//	void OnExperienceLoaded(const UCVExperienceDefinition* CurrentExperience);
//
//	/** 서버 전용: PawnData 설정(Replicate) */
//	UFUNCTION(BlueprintCallable, Category = "CV|PlayerState")
//	void SetPawnData_ServerOnly(const UCVPawnData* InPawnData);
//
//	// ───── ASC Access ─────
//	UFUNCTION(BlueprintCallable, Category = "CV|ASC")
//	UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const;
//
//	/** GAS 표준 인터페이스 구현 */
//	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
//	{
//		return AbilitySystemComponent;
//	}
//
//	// AActor
//	virtual void PostInitializeComponents() override;
//
//protected:
//	// Replication
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//	UFUNCTION()
//	void OnRep_PawnData();
//
//protected:
//	/** 게임 진행 중 사용할 PawnData. 스폰/로그인 시 서버가 지정 → 클라에 복제 */
//	UPROPERTY(ReplicatedUsing = OnRep_PawnData, EditDefaultsOnly, Category = "CV|PlayerState")
//	TObjectPtr<const UCVPawnData> PawnData = nullptr;
//
//	/** 서버 소유 ASC (인터페이스 요구사항으로 base 타입 유지) */
//	UPROPERTY(VisibleAnywhere, Category = "CV|PlayerState")
//	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
//};



#pragma once

#include "AbilitySystemInterface.h"
#include "Game/AbilitySystem/CVAbilitySet.h"
#include "GameFramework/PlayerState.h"
#include "CVPlayerState.generated.h"

class ACVPlayerController;
class ACVCharacter;
class UCVAbilitySystemComponent;

UCLASS()
class CHRONOVEIL_API ACVPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACVPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "CVPlayerState")
	ACVPlayerController* GetCVPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "CVPlayerState")
	ACVCharacter* GetCVCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "CVPlayerState")
	UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


private:
	UPROPERTY(VisibleAnywhere, Category = "CVPlayerState|AbilitySystemComponent")
	TObjectPtr<UCVAbilitySystemComponent> AbilitySystemComponent;

	UFUNCTION()
	void OnReadyPawnData(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	FCVAbilitySet_GrantedHandles GrantedHandles;
};
