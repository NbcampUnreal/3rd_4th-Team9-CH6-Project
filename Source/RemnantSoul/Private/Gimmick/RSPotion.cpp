// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RSPotion.h"
#include "Interface/InventoryOwner.h"


// Sets default values
ARSPotion::ARSPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	Body->SetupAttachment(Root);
	
	// 기본 충돌 세팅: 월드에 놓이고 라인트레이스로 맞게 하고 싶으면 Visibility에 Block이 필요함
	Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Body->SetCollisionObjectType(ECC_WorldDynamic);
	
	Body->SetCollisionResponseToAllChannels(ECR_Ignore);
	Body->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Body->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	Body->SetGenerateOverlapEvents(false);
	
	// 아이템 데이터는 에디터에서 지정 (EditInstanceOnly)
	ItemData = nullptr;
	
}

bool ARSPotion::CanInteract_Implementation(AActor* Interactor) const
{
	return ItemData != nullptr;
}

void ARSPotion::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log,
		TEXT("[Potion] Interact called by %s"),
		Interactor ? *Interactor->GetName() : TEXT("NULL")
	);
	if (!ItemData || !Interactor)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Potion] Invalid ItemData or Interactor"));
		return;
	}

	if (Interactor->Implements<UInventoryOwner>())
	{
		UE_LOG(LogTemp, Log,
			TEXT("[Potion] Interactor implements InventoryOwner"));
		const bool bAdded = IInventoryOwner::Execute_TryAddItem(Interactor, ItemData, 1);
		UE_LOG(LogTemp, Log,
			TEXT("[Potion] TryAddItem result: %s"),
			bAdded ? TEXT("SUCCESS") : TEXT("FAILED"));
		if (bAdded)
		{
			Destroy();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Potion] Interactor does NOT implement InventoryOwner"));
	}

	
}

URSItemData* ARSPotion::GetItemData_Implementation() const
{
	return ItemData;
}
