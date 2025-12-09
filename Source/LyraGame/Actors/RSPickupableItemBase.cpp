//#include "RSPickupableItemBase.h"
//
//#include "Components/BoxComponent.h"
//#include "Data/RSItemData.h"
//#include "GameFramework/ProjectileMovementComponent.h"
//#include "Item/RSItemInstance.h"
//#include "Item/Fragments/RSItemFragment_Equipable.h"
//#include "Kismet/KismetSystemLibrary.h"
//#include "System/LyraAssetManager.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSPickupableItemBase)
//
//ARSPickupableItemBase::ARSPickupableItemBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	
//	bReplicates = true;
//	bAlwaysRelevant = true;
//	AActor::SetReplicateMovement(true);
//
//	MovementCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
//	MovementCollision->SetCollisionProfileName("BlockOnlyWorldObject");
//	SetRootComponent(MovementCollision);
//
//	PickupCollision = CreateDefaultSubobject<UBoxComponent>("PickupCollision");
//	PickupCollision->SetCollisionProfileName("Pickupable");
//	PickupCollision->SetupAttachment(GetRootComponent());
//	
//    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
//	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//	MeshComponent->SetCollisionProfileName("NoCollision");
//	MeshComponent->SetupAttachment(GetRootComponent());
//
//	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
//	ProjectileMovement->bShouldBounce = true;
//	ProjectileMovement->Bounciness = 0.f;
//	ProjectileMovement->Friction = 1.f;
//	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 0.f;
//	ProjectileMovement->Velocity = FVector::ZeroVector;
//}
//
//void ARSPickupableItemBase::OnRep_PickupInfo()
//{
//	Super::OnRep_PickupInfo();
//
//	TSoftObjectPtr<UStaticMesh> PickupableMeshPath = nullptr;
//		
//	if (const URSItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance)
//	{
//		const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
//		PickupableMeshPath = ItemTemplate.PickupableMesh;
//	}
//	else if (TSubclassOf<URSItemTemplate> ItemTemplateClass = PickupInfo.PickupTemplate.ItemTemplateClass)
//	{
//		const URSItemTemplate* ItemTemplate = ItemTemplateClass->GetDefaultObject<URSItemTemplate>();
//		PickupableMeshPath = ItemTemplate->PickupableMesh;
//	}
//
//	if (PickupableMeshPath.IsNull() == false)
//	{
//		if (UStaticMesh* PickupableMesh = ULyraAssetManager::GetAssetByPath(PickupableMeshPath))
//		{
//			MeshComponent->SetStaticMesh(PickupableMesh);
//
//			if (bAutoCollisionResize)
//			{
//				float Radius;
//				FVector Origin, BoxExtent;
//				UKismetSystemLibrary::GetComponentBounds(MeshComponent, Origin, BoxExtent, Radius);
//
//				FVector MovementCollisionExtent = FVector(FMath::Min(MaxMovementCollisionExtent.X, BoxExtent.X), FMath::Min(MaxMovementCollisionExtent.Y, BoxExtent.Y), BoxExtent.Z);
//				MovementCollision->SetBoxExtent(MovementCollisionExtent);
//				
//				FVector PickupCollisionExtent = FVector(FMath::Max(MinPickupCollisionExtent.X, BoxExtent.X), FMath::Max(MinPickupCollisionExtent.Y, BoxExtent.Y), BoxExtent.Z);
//				PickupCollision->SetBoxExtent(PickupCollisionExtent);
//			}
//		}
//	}
//}
//
//void ARSPickupableItemBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
//{
//	OutMeshComponents.Add(MeshComponent);
//}
