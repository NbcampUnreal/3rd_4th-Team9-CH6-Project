// 1.
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/PawnComponent.h"
//#include "Components/GameFrameworkInitStateInterface.h" 
//#include "CVPawnExtensionComponent.generated.h"
//
//class UCVAbilitySystemComponent;
//class UCVPawnData;
//class ACVCharacter;
//
//
///**
// * 경량 Lyra 패턴: Pawn 초기화 상태(InitState) 통합 + ASC 연결
// * - Feature 등록/해제
// * - PawnData 세팅 감지 → InitState 선형 진행
// * - ASC Init/Uninit 및 Delegate 브로드캐스트
// */
//UCLASS(ClassGroup = (CV), meta = (BlueprintSpawnableComponent))
//class CHRONOVEIL_API UCVPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
//{
//	GENERATED_BODY()
//
//public:
//	UCVPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	/** GameFrameworkComponentManager에 등록할 FeatureName */
//	static const FName NAME_ActorFeatureName;
//
//	/** 편의 함수 */
//	static UCVPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
//	{
//		return (Actor ? Actor->FindComponentByClass<UCVPawnExtensionComponent>() : nullptr);
//	}
//
//	template <class T>
//	const T* GetPawnData() const { return Cast<T>(PawnData); }
//
//	/** 서버 권한에서만 PawnData 설정 */
//	void SetPawnData(const UCVPawnData* InPawnData);
//
//	/** 입력 재설정이 필요할 때 InitState 체인 재평가 트리거 */
//	void SetupPlayerInputComponent();
//
//	/** ASC 초기화/해제 */
//	UCVAbilitySystemComponent* GetCVAbilitySystemComponent() const { return AbilitySystemComponent; }
//	void InitializeAbilitySystem(UCVAbilitySystemComponent* InASC, AActor* InOwnerActor);
//	void UninitializeAbilitySystem();
//
//	/** ASC 초기화/해제 시점 Delegate 등록(한 객체 1회) */
//	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
//	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
//
//	/** 서버 포제션 시 ASC 초기화 + AbilitySet 부여 */
//	void HandlePossessed(ACVCharacter* Char);
//
//	// ---------- UPawnComponent ----------
//	virtual void OnRegister() final;
//	virtual void BeginPlay() final;
//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;
//
//	// ---------- IGameFrameworkInitStateInterface ----------
//	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
//	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
//	virtual bool CanChangeInitState(class UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
//	virtual void CheckDefaultInitialization() final;
//
//public:
//	/** Pawn을 생성 시점에 주입하는 데이터(InitialOnly로 복제되는 것을 권장) */
//	UPROPERTY(EditInstanceOnly, Category = "CV|Pawn")
//	TObjectPtr<const UCVPawnData> PawnData = nullptr;
//
//	/** ASC 캐시 */
//	UPROPERTY()
//	TObjectPtr<UCVAbilitySystemComponent> AbilitySystemComponent = nullptr;
//
//	/** ASC Init/Uninit delegate */
//	FSimpleMulticastDelegate OnAbilitySystemInitialized;
//	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
//};
