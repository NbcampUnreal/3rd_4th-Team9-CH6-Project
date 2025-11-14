
// 1.
//#include "Game/Character/CVPawnExtensionComponent.h"
//
//#include "Game/Character/CVCharacter.h"
//#include "Game/Player/CVPlayerState.h"
//#include "Game/AbilitySystem/CVAbilitySystemComponent.h"
//#include "Game/AbilitySystem/CVAbilitySet.h"
//#include "Game/Character/CVPawnData.h"
//#include "Game/CVLogChannels.h"
//#include "Game/CVGameplayTags.h"
//#include "GameplayTagContainer.h" 
//#include "Components/GameFrameworkComponentManager.h"
//
//const FName UCVPawnExtensionComponent::NAME_ActorFeatureName(TEXT("PawnExtension"));
//
//UCVPawnExtensionComponent::UCVPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryComponentTick.bStartWithTickEnabled = false;
//	PrimaryComponentTick.bCanEverTick = false;
//}
//
///** 서버 권한에서만 1회 세팅 */
//void UCVPawnExtensionComponent::SetPawnData(const UCVPawnData* InPawnData)
//{
//	APawn* Pawn = GetPawn<APawn>();
//	if (!Pawn || Pawn->GetLocalRole() != ROLE_Authority)
//	{
//		return;
//	}
//	if (PawnData) { return; }
//
//	PawnData = InPawnData;
//
//	// PawnData가 들어오면 InitState 진행 가능성이 생김
//	CheckDefaultInitialization();
//}
//
//void UCVPawnExtensionComponent::SetupPlayerInputComponent()
//{
//	// 입력(매핑/IMC) 구조가 바뀌었을 수 있으므로 InitState 재평가 트리거
//	CheckDefaultInitialization();
//}
//
//void UCVPawnExtensionComponent::InitializeAbilitySystem(UCVAbilitySystemComponent* InASC, AActor* InOwnerActor)
//{
//	check(InASC && InOwnerActor);
//
//	if (AbilitySystemComponent == InASC)
//	{
//		return;
//	}
//
//	if (AbilitySystemComponent)
//	{
//		UninitializeAbilitySystem();
//	}
//
//	APawn* Pawn = GetPawn<APawn>();
//	check(Pawn);
//
//	if (AActor* ExistingAvatar = InASC->GetAvatarActor())
//	{
//		if (ExistingAvatar != GetOwner())
//		{
//			InASC->ClearActorInfo(); // or 안전한 Uninit 경로
//		}
//	}
//
//	AbilitySystemComponent = InASC;
//	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
//
//	// ASC Init 알림
//	OnAbilitySystemInitialized.Broadcast();
//}
//
//void UCVPawnExtensionComponent::UninitializeAbilitySystem()
//{
//	if (!AbilitySystemComponent)
//	{
//		return;
//	}
//
//	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
//	{
//		// ASC Uninit 알림
//		OnAbilitySystemUninitialized.Broadcast();
//	}
//
//	AbilitySystemComponent = nullptr;
//}
//
//void UCVPawnExtensionComponent::OnRegister()
//{
//	Super::OnRegister();
//
//	// 올바른 소유자 확인
//	if (!GetPawn<APawn>())
//	{
//		UE_LOG(LogCV, Error, TEXT("CVPawnExtensionComponent must be attached to a Pawn!"));
//		return;
//	}
//
//	// GameFrameworkComponentManager에 Feature 등록 (InitState 관리용)
//	RegisterInitStateFeature();
//
//#if !UE_BUILD_SHIPPING
//	// 디버깅용으로 매니저 핸들 포착(미사용)
//	UGameFrameworkComponentManager::GetForActor(GetOwningActor());
//#endif
//}
//
//void UCVPawnExtensionComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//	// Actor의 Feature 상태 변화를 관찰 (NAME_None = 모든 Feature)
//	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), /*bRunImmediately*/false);
//
//	// InitState: Spawned (0단계)
//	const auto& Tags = FCVGameplayTags::Get();
//	ensure(TryToChangeInitState(FCVGameplayTags::Get().InitState_Spawned)); // 아래에서 InitState 태그 정의
//
//	// 강제 업데이트(선형 체인)
//	CheckDefaultInitialization();
//}
//
//void UCVPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//	// OnRegister 시 등록한 Feature를 해제
//	UnregisterInitStateFeature();
//
//	Super::EndPlay(EndPlayReason);
//}
//
//void UCVPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
//{
//	// 본 컴포넌트 외 Feature들이 DataAvailable에 도달하는 순간을 캐치 → 우리 체인 재평가
//	if (Params.FeatureName != NAME_ActorFeatureName)
//	{
//		const auto& Init = FCVGameplayTags::Get();
//		if (Params.FeatureState == Init.InitState_DataAvailable)
//		{
//			CheckDefaultInitialization();
//		}
//	}
//}
//
//bool UCVPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
//{
//	check(Manager);
//
//	APawn* Pawn = GetPawn<APawn>();
//	const auto& Init = FCVGameplayTags::Get();
//
//	// (1) Invalid -> Spawned
//	if (!CurrentState.IsValid() && DesiredState == Init.InitState_Spawned)
//	{
//		return (Pawn != nullptr);
//	}
//
//	// (2) Spawned -> DataAvailable : PawnData가 있어야 함 (+로컬이면 Controller 필요)
//	if (CurrentState == Init.InitState_Spawned && DesiredState == Init.InitState_DataAvailable)
//	{
//		if (!PawnData)
//		{
//			return false;
//		}
//		if (Pawn && Pawn->IsLocallyControlled())
//		{
//			if (!GetController<AController>())
//			{
//				return false;
//			}
//		}
//		return true;
//	}
//
//	// (3) DataAvailable -> DataInitialized : 모든 Feature가 DataAvailable에 도달해야 함
//	if (CurrentState == Init.InitState_DataAvailable && DesiredState == Init.InitState_DataInitialized)
//	{
//		return Manager->HaveAllFeaturesReachedInitState(Pawn, Init.InitState_DataAvailable);
//	}
//
//	// (4) DataInitialized -> GameplayReady : 즉시 가능(세부 조건은 각 컴포넌트가 담당)
//	if (CurrentState == Init.InitState_DataInitialized && DesiredState == Init.InitState_GameplayReady)
//	{
//		return true;
//	}
//
//	// 그 외 비선형 전이는 불가
//	return false;
//}
//
//void UCVPawnExtensionComponent::CheckDefaultInitialization()
//{
//	// 본인 포함 Feature 구현체들에 InitState 재평가 기회 제공
//	CheckDefaultInitializationForImplementers();
//
//	const auto& Init = FCVGameplayTags::Get();
//
//	// 선형 체인 정의
//	static const TArray<FGameplayTag> StateChain = {
//		Init.InitState_Spawned,
//		Init.InitState_DataAvailable,
//		Init.InitState_DataInitialized,
//		Init.InitState_GameplayReady
//	};
//
//	// 가능한 만큼 앞으로 진행(멈추면 외부 트리거 필요)
//	ContinueInitStateChain(StateChain);
//}
//
//void UCVPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
//{
//	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
//	{
//		OnAbilitySystemInitialized.Add(Delegate);
//	}
//
//	if (AbilitySystemComponent)
//	{
//		Delegate.Execute();
//	}
//}
//
//void UCVPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
//{
//	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
//	{
//		OnAbilitySystemUninitialized.Add(Delegate);
//	}
//}
//
//// 서버: 포제션 직후 호출해서 ASC 초기화 및 AbilitySet 부여
//void UCVPawnExtensionComponent::HandlePossessed(ACVCharacter* Char)
//{
//	if (!Char || !Char->HasAuthority())
//	{
//		return;
//	}
//
//	// 1) PlayerState에서 ASC 구하기 (프로젝트 스타일에 맞춰 선택)
//	// (A) PlayerState가 전용 getter를 제공하는 경우:
//	APlayerState* PS = Char->GetPlayerState();
//	if (!PS) return;
//
//	UCVAbilitySystemComponent* ASC = nullptr;
//
//	if (ACVPlayerState* CVPS = Cast<ACVPlayerState>(PS))
//	{
//		// 1) 전용 getter가 있다면 이걸로 끝
//		ASC = CVPS->GetCVAbilitySystemComponent();
//
//		// 2) 전용 getter가 없다면 이렇게 캐스팅
//		if (!ASC)
//		{
//			ASC = Cast<UCVAbilitySystemComponent>(CVPS->GetAbilitySystemComponent());
//		}
//	}
//	// (B) 또는 FindComponentByClass로 얻는 방식:
//	if (!ASC)
//	{
//		ASC = PS->FindComponentByClass<UCVAbilitySystemComponent>();
//	}
//	if (!ASC) return;
//
//	// 2) ASC 초기화 (Owner=PS, Avatar=Char)
//	InitializeAbilitySystem(ASC, PS);
//
//	// 3) AbilitySet 부여 (PawnData가 세팅되어 있다는 가정)
//	if (const UCVPawnData* Data = PawnData)
//	{
//		for (const UCVAbilitySet* Set : Data->AbilitySets)
//		{
//			if (Set)
//			{
//				Set->GiveToAbilitySystem(ASC, /*OutGrantedHandles*/ nullptr, /*Source*/ Char);
//			}
//		}
//	}
//
//	// 필요 시 여기서 InitState 체인 한 번 더 밀어주기
//	CheckDefaultInitialization();
//}