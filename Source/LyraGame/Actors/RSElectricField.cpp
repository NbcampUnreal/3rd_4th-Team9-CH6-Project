//#include "RSElectricField.h"
//
//#include "GameModes/LyraGameState.h"
//#include "Kismet/GameplayStatics.h"
//#include "System/RSElectricFieldManagerComponent.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSElectricField)
//
//class ALyraGameState;
//
//ARSElectricField::ARSElectricField(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//
//	bReplicates = true;
//	bAlwaysRelevant = true;
//	AActor::SetReplicateMovement(true);
//	
//	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
//	MeshComponent->SetIsReplicated(true);
//	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	SetRootComponent(MeshComponent);
//}
//
//void ARSElectricField::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (ALyraGameState* LyraGameState = Cast<ALyraGameState>(UGameplayStatics::GetGameState(this)))
//	{
//		if (URSElectricFieldManagerComponent* ElectricFieldManager = LyraGameState->FindComponentByClass<URSElectricFieldManagerComponent>())
//		{
//			ElectricFieldManager->ElectricFieldActor = this;
//		}
//	}
//}
