//#include "Game/Player/CVPlayerState.h"
//#include "Game/GameModes/CVExperienceManagerComponent.h"
//#include "Game/GameModes/CVGameModeBase.h"
//#include "Game/GameModes/CVExperienceDefinition.h"
//
//#include "Game/Character/CVPawnData.h"
//#include "Game/AbilitySystem/CVAbilitySet.h"
//#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
//
//#include "GameFramework/GameStateBase.h"
//#include "GameFramework/Pawn.h"
//#include "Net/UnrealNetwork.h"
//#include "TimerManager.h"
//
//ACVPlayerState::ACVPlayerState()
//{
//	AbilitySystemComponent = CreateDefaultSubobject<UCVAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
//	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
//}
//
//void ACVPlayerState::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//	check(AbilitySystemComponent);
//	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
//
//	// 경험 로딩 완료 신호 구독
//	if (const AGameStateBase* GS = GetWorld()->GetGameState())
//	{
//		if (auto* XMan = GS->FindComponentByClass<UCVExperienceManagerComponent>())
//		{
//			XMan->CallOrRegister_OnExperienceLoaded(
//				FOnCVExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded)
//			);
//		}
//	}
//}
//
//void ACVPlayerState::OnExperienceLoaded(const UCVExperienceDefinition* /*CurrentExperience*/)
//{
//	// 서버에서만 PawnData 확정 및 Ability 부여
//	if (!HasAuthority())
//	{
//		return;
//	}
//
//	// 1) GameMode로부터 최종 PawnData 조회
//	const UCVPawnData* NewPawnData = nullptr;
//	if (ACVGameModeBase* GM = GetWorld()->GetAuthGameMode<ACVGameModeBase>())
//	{
//		NewPawnData = GM->GetPawnDataForController(GetOwningController());
//	}
//
//	// 2) 실패 시 Experience 기본값으로 보조 루트
//	if (!NewPawnData)
//	{
//		if (const AGameStateBase* GS = GetWorld()->GetGameState())
//		{
//			if (const UCVExperienceManagerComponent* XMan = GS->FindComponentByClass<UCVExperienceManagerComponent>())
//			{
//				if (XMan->IsExperienceLoaded())
//				{
//					if (const UCVExperienceDefinition* Exp = XMan->GetCurrentExperienceChecked())
//					{
//						NewPawnData = Exp->GetDefaultPawnData();
//					}
//				}
//			}
//		}
//	}
//
//	// 3) 실제 적용
//	if (NewPawnData && NewPawnData != PawnData)
//	{
//		SetPawnData_ServerOnly(NewPawnData);
//
//		if (AbilitySystemComponent && PawnData)
//		{
//			// AbilitySet 일괄 부여(중복 방지 로직은 Set 내부/ASC 측에서 처리 가능)
//			if (UCVAbilitySystemComponent* CVASC = GetCVAbilitySystemComponent())
//			{
//				for (UCVAbilitySet* Set : PawnData->AbilitySets)
//				{
//					if (Set) { Set->GiveToAbilitySystem(CVASC, nullptr); }
//				}
//			}
//
//			// Pawn이 있으면 AvatarInfo 확정, 없으면 다음 틱에 재시도
//			if (APawn* P = GetPawn())
//			{
//				AbilitySystemComponent->InitAbilityActorInfo(this, P);
//			}
//			else
//			{
//				FTimerHandle Tmp;
//				GetWorld()->GetTimerManager().SetTimer(
//					Tmp,
//					[this]()
//					{
//						if (AbilitySystemComponent && GetPawn())
//						{
//							AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
//						}
//					},
//					0.0f, false
//				);
//			}
//		}
//	}
//}
//
//void ACVPlayerState::SetPawnData_ServerOnly(const UCVPawnData* InPawnData)
//{
//	check(HasAuthority());
//	check(InPawnData);
//	if (PawnData == InPawnData) return;
//
//	PawnData = InPawnData;
//	OnRep_PawnData();
//}
//
//void ACVPlayerState::OnRep_PawnData()
//{
//	// 클라 HUD/UI 반영이 필요하면 여기서
//}
//
//void ACVPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(ACVPlayerState, PawnData);
//}
//
//UCVAbilitySystemComponent* ACVPlayerState::GetCVAbilitySystemComponent() const
//{
//	return Cast<UCVAbilitySystemComponent>(AbilitySystemComponent);
//}



#include "Game/Player/CVPlayerState.h"
#include "Game/Player/CVPlayerController.h"
#include "Game/Character/CVCharacter.h"
#include "Game/Character/CVPawnData.h"
#include "Game/AbilitySystem/CVAbilitySystemComponent.h"

ACVPlayerState::ACVPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UCVAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystem 네트워크 관련: needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	// PlayerState와 Pawn(Chracter)가 모두 준비되었을 때 호출되는 Delegate
	OnPawnSet.AddDynamic(this, &ThisClass::OnReadyPawnData);
}

ACVPlayerController* ACVPlayerState::GetCVPlayerController() const
{
	return Cast<ACVPlayerController>(GetOwner());
}

ACVCharacter* ACVPlayerState::GetCVCharacter() const
{
	return Cast<ACVCharacter>(GetPawn());
}

UAbilitySystemComponent* ACVPlayerState::GetAbilitySystemComponent() const
{
	return GetCVAbilitySystemComponent();
}

void ACVPlayerState::OnReadyPawnData(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	ACVCharacter* CVCharacter = GetCVCharacter();
	if (IsValid(CVCharacter))
	{
		AActor* ComponentOwner = this;
		AActor* Avatar = CVCharacter;
		AbilitySystemComponent->InitAbilityActorInfo(ComponentOwner, Avatar);

		const UCVPawnData* PawnData = CVCharacter->GetPawnData();
		if (IsValid(PawnData))
		{
			for (const UCVAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
			}
		}
	}
}
