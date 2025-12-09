//#include "RSDoorBase.h"
//
//#include "Components/ArrowComponent.h"
//#include "Net/UnrealNetwork.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSDoorBase)
//
//ARSDoorBase::ARSDoorBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
//	SetRootComponent(ArrowComponent);
//
//	LeftMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftMeshComponent"));
//	LeftMeshComponent->SetupAttachment(GetRootComponent());
//	LeftMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
//	LeftMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//	LeftMeshComponent->SetCanEverAffectNavigation(true);
//
//	RightMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightMeshComponent"));
//	RightMeshComponent->SetupAttachment(GetRootComponent());
//	RightMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
//	RightMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//	RightMeshComponent->SetCanEverAffectNavigation(true);
//}
//
//void ARSDoorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, DoorState);
//}
//
//FRSInteractionInfo ARSDoorBase::GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const
//{
//	switch (DoorState)
//	{
//	case EDoorState::Open_Forward:
//	case EDoorState::Open_Backward:
//		return OpenedInteractionInfo;
//		
//	case EDoorState::Close:
//		return ClosedInteractionInfo;
//		
//	default:
//		return FRSInteractionInfo();
//	}
//}
//
//void ARSDoorBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
//{
//	if (LeftMeshComponent->GetStaticMesh())
//	{
//		OutMeshComponents.Add(LeftMeshComponent);
//	}
//
//	if (RightMeshComponent->GetStaticMesh())
//	{
//		OutMeshComponents.Add(RightMeshComponent);
//	}
//}
//
//void ARSDoorBase::SetDoorState(EDoorState NewDoorState)
//{
//	if (HasAuthority() == false || NewDoorState == DoorState)
//		return;
//
//	DoorState = NewDoorState;
//	OnRep_DoorState();
//}
//
//void ARSDoorBase::OnRep_DoorState()
//{
//	OnDoorStateChanged(DoorState);
//}
