//#include "RSStatueBase.h"
//
//#include "Components/ArrowComponent.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSStatueBase)
//
//ARSStatueBase::ARSStatueBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	
//	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
//	SetRootComponent(ArrowComponent);
//	
//	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
//	MeshComponent->SetupAttachment(GetRootComponent());
//	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
//	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//
//	bCanUsed = true;
//}
//
//FRSInteractionInfo ARSStatueBase::GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const
//{
//	return InteractionInfo;
//}
//
//void ARSStatueBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
//{
//	OutMeshComponents.Add(MeshComponent);
//}
