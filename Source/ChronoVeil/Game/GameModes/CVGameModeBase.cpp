//#include "Game/GameModes/CVGameModeBase.h"
//#include "Game/GameModes/CVGameStateBase.h"
//#include "Game/GameModes/CVExperienceManagerComponent.h"
//#include "Game/GameModes/CVExperienceDefinition.h"
//
//#include "Game/Character/CVCharacter.h"
//#include "Game/Character/CVPawnData.h"
//#include "Game/Character/CVPawnExtensionComponent.h"
//#include "Game/Player/CVPlayerController.h"
//#include "Game/Player/CVPlayerState.h"
//
//#include "Kismet/GameplayStatics.h"
//#include "Engine/World.h"
//#include "TimerManager.h"
//
//ACVGameModeBase::ACVGameModeBase()
//{
//	GameStateClass = ACVGameStateBase::StaticClass();
//	PlayerControllerClass = ACVPlayerController::StaticClass();
//	PlayerStateClass = ACVPlayerState::StaticClass();
//	DefaultPawnClass = ACVCharacter::StaticClass();
//}
//
//void ACVGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
//{
//	Super::InitGame(MapName, Options, ErrorMessage);
//
//	// 경험 처리는 다음 프레임으로 미루기(OptionsString 사용 가능)
//	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
//}
//
//void ACVGameModeBase::InitGameState()
//{
//	Super::InitGameState();
//
//	auto* XMan = GameState->FindComponentByClass<UCVExperienceManagerComponent>();
//	check(XMan);
//
//	// UCVExperienceManagerComponent::FOnCVExperienceLoaded::FDelegate::CreateUObject(...)  (틀림)
//	// 전역 델리게이트 타입을 바로 사용
//	XMan->CallOrRegister_OnExperienceLoaded(
//		FOnCVExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded)
//	);
//}
//
//UClass* ACVGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
//{
//	if (const UCVPawnData* PD = GetPawnDataForController(InController))
//	{
//		if (PD->PawnClass) { return PD->PawnClass; }
//	}
//	return Super::GetDefaultPawnClassForController_Implementation(InController);
//}
//
//void ACVGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
//{
//	// 경험 로딩 전이면 보류
//	if (!IsExperienceLoaded())
//	{
//		return;
//	}
//	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
//
//	// 안전망: Pawn이 이미 있고 PawnData 미세팅이면 주입
//	if (APawn* P = NewPlayer ? NewPlayer->GetPawn() : nullptr)
//	{
//		if (auto* Ext = P->FindComponentByClass<UCVPawnExtensionComponent>())
//		{
//			if (!Ext->GetPawnData<UCVPawnData>())
//			{
//				if (const UCVPawnData* PD = GetPawnDataForController(NewPlayer))
//				{
//					Ext->SetPawnData(PD);
//				}
//			}
//		}
//	}
//}
//
//APawn* ACVGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& Xform)
//{
//	FActorSpawnParameters Params;
//	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//	Params.Instigator = GetInstigator();
//	Params.ObjectFlags |= RF_Transient;
//	Params.bDeferConstruction = true;
//
//	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
//	{
//		if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(PawnClass, Xform, Params))
//		{
//			if (auto* Ext = Pawn->FindComponentByClass<UCVPawnExtensionComponent>())
//			{
//				if (const UCVPawnData* PD = GetPawnDataForController(NewPlayer))
//				{
//					Ext->SetPawnData(PD);
//				}
//			}
//			Pawn->FinishSpawning(Xform);
//			return Pawn;
//		}
//	}
//	return nullptr;
//}
//
//// ---------- 경험 선택/적용 ----------
//
//// 1. 
//void ACVGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
//{
//	FPrimaryAssetId ExperienceId;
//
//	// 1) URL 옵션 우선: ?Experience=BPName
//	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
//	{
//		const FString Name = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
//		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(TEXT("CVExperienceDefinition")), FName(*Name));
//	}
//
//	// 2) 그래도 없으면(테스트용) 프로젝트 기본값(있다면)로… 없으면 그대로 종료
//	if (!ExperienceId.IsValid())
//	{
//		//UE_LOG(LogTemp, Warning, TEXT("[CV] No Experience in OptionsString; using GameMode fallback if any"));
//		//// 필요 시 GameMode에 UPROPERTY로 DefaultExperienceId 두고 채워도 됨
//		//return;
//
//		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("CVExperienceDefinition"), FName("B_CVDefaultExperience"));
//	}
//
//	OnMatchAssignmentGiven(ExperienceId);
//}
//
//void ACVGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
//{
//	check(ExperienceId.IsValid());
//
//	auto* XMan = GameState->FindComponentByClass<UCVExperienceManagerComponent>();
//	check(XMan);
//	XMan->ServerSetCurrentExperience(ExperienceId);
//}
//
//bool ACVGameModeBase::IsExperienceLoaded() const
//{
//	check(GameState);
//	auto* XMan = GameState->FindComponentByClass<UCVExperienceManagerComponent>();
//	check(XMan);
//	return XMan->IsExperienceLoaded();
//}
//
//void ACVGameModeBase::OnExperienceLoaded(const UCVExperienceDefinition* CurrentExperience)
//{
//	// 경험 로딩 완료: Pawn 없는 컨트롤러 재시작
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		APlayerController* PC = Cast<APlayerController>(*It);
//		if (PC && PC->GetPawn() == nullptr && PlayerCanRestart(PC))
//		{
//			RestartPlayer(PC);
//		}
//	}
//}
//
//const UCVPawnData* ACVGameModeBase::GetPawnDataForController(const AController* InController) const
//{
//	// 1) PlayerState 우선
//	if (InController)
//	{
//		if (const ACVPlayerState* PS = InController->GetPlayerState<ACVPlayerState>())
//		{
//			if (const UCVPawnData* FromPS = PS->GetPawnData<UCVPawnData>())
//			{
//				return FromPS;
//			}
//		}
//	}
//
//	// 2) 현재 Experience의 DefaultPawnData
//	if (const AGameStateBase* GS = GetWorld()->GetGameState())
//	{
//		if (const UCVExperienceManagerComponent* XMan = GS->FindComponentByClass<UCVExperienceManagerComponent>())
//		{
//			if (XMan->IsExperienceLoaded())
//			{
//				const UCVExperienceDefinition* Exp = XMan->GetCurrentExperienceChecked();
//				if (const UCVPawnData* FromExp = Exp ? Exp->GetDefaultPawnData() : nullptr)
//				{
//					return FromExp;
//				}
//			}
//		}
//	}
//
//	// 3) 마지막 보루: GameMode 기본값
//	return DefaultPawnData;
//}


#include "Game/GameModes/CVGameModeBase.h"

