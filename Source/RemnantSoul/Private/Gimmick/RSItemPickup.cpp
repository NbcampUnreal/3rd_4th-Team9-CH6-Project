// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/RSItemPickup.h"
#include "Interface/InventoryOwner.h"

#include "Components/StaticMeshComponent.h"
#include "ItemDataAsset/RSItemData.h"

ARSItemPickup::ARSItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	// 기본 충돌 세팅: 월드에 놓이고 라인트레이스로 맞게 하고 싶으면 Visibility Block 필요
	Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Body->SetCollisionObjectType(ECC_WorldDynamic);

	Body->SetCollisionResponseToAllChannels(ECR_Ignore);
	Body->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Body->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Body->SetGenerateOverlapEvents(false);

	ItemData = nullptr;
}

bool ARSItemPickup::CanInteract_Implementation(AActor* Interactor) const
{
	return ItemData != nullptr;
}

void ARSItemPickup::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[ItemPickup] Interact by %s"), *GetNameSafe(Interactor));

	if (!ItemData || !Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Invalid ItemData or Interactor"));
		return;
	}

	if (!Interactor->Implements<UInventoryOwner>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPickup] Interactor does NOT implement InventoryOwner"));
		return;
	}

	const bool bAdded = IInventoryOwner::Execute_TryAddItem(Interactor, ItemData, 1);
	UE_LOG(LogTemp, Log, TEXT("[ItemPickup] TryAddItem: %s"), bAdded ? TEXT("SUCCESS") : TEXT("FAILED"));

	if (bAdded)
	{
		Destroy();
	}
}

URSItemData* ARSItemPickup::GetItemData_Implementation() const
{
	return ItemData;
}

void ARSItemPickup::SetHighlight(bool bEnable)
{
	if (!Body)
	{
		return;
	}

	Body->SetRenderCustomDepth(bEnable);

	// 선택: 스텐실 값도 같이 (프로젝트/PP 설정에 맞춰)
	if (bEnable)
	{
		Body->SetCustomDepthStencilValue(1);
	}
}
