//#include "RSTargetPointBase.h"
//
//#include "GameModes/LyraExperienceManagerComponent.h"
//#include "Kismet/GameplayStatics.h"
//#include "System/RSActorSpawningManagerComponent.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSTargetPointBase)
//
//ARSTargetPointBase::ARSTargetPointBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	PrimaryActorTick.bStartWithTickEnabled = false;
//	
//	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
//	PreviewMeshComponent->SetupAttachment(RootComponent);
//	PreviewMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
//	PreviewMeshComponent->SetHiddenInGame(true);
//	PreviewMeshComponent->bIsEditorOnly = true;
//}
//
//void ARSTargetPointBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (HasAuthority() == false)
//		return;
//
//	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
//	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
//	check(ExperienceComponent);
//	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
//}
//
//void ARSTargetPointBase::OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition)
//{
//	if (HasAuthority() == false)
//		return;
//	
//	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
//	URSActorSpawningManagerComponent* ActorSpawningManager = GameState->FindComponentByClass<URSActorSpawningManagerComponent>();
//	check(ActorSpawningManager);
//	ActorSpawningManager->RegisterTargetPoint(this);
//}
//
//AActor* ARSTargetPointBase::SpawnActor()
//{
//	if (HasAuthority() == false)
//		return nullptr;
//
//	FTransform SpawnTransform = GetActorTransform();
//	SpawnTransform.AddToTranslation(SpawnLocationOffset);
//	
//	AActor* SpawningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
//	InitializeSpawningActor(SpawningActor);
//	SpawningActor->FinishSpawning(SpawnTransform);
//	SpawnedActor = SpawningActor;
//	
//	return SpawningActor;
//}
//
//void ARSTargetPointBase::DestroyActor()
//{
//	if (HasAuthority() == false)
//		return;
//	
//	if (SpawnedActor.Get())
//	{
//		SpawnedActor->Destroy(true);
//	}
//}
