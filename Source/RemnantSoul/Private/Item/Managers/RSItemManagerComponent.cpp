#include "Item/Managers/RSItemManagerComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#include "Character/RSCharacter.h"
#include "Item/Managers/RSInventoryManagerComponent.h"
#include "Item/Managers/RSEquipmentManagerComponent.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"


#include "Item/Managers/RSEquipManagerComponent.h"
#include "Character/RSCombatStyleData.h"
#include "RSGameplayTags.h"
#include "AbilitySystemComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemManagerComponent)

URSItemManagerComponent::URSItemManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSItemManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheControllerAndPawn();
}

void URSItemManagerComponent::CacheControllerAndPawn()
{
	if (CachedPC.IsValid() && CachedPawn.IsValid())
	{
		return;
	}

	AActor* Owner = GetOwner();
	APlayerController* PC = Cast<APlayerController>(Owner);
	if (!PC)
	{
		return;
	}

	CachedPC = PC;

	APawn* Pawn = PC->GetPawn();
	if (ARSCharacter* RSChar = Cast<ARSCharacter>(Pawn))
	{
		CachedPawn = RSChar;

		// Pawn 쪽 매니저들 캐싱
		if (URSInventoryManagerComponent* InvMgr = RSChar->FindComponentByClass<URSInventoryManagerComponent>())
		{
			CachedInventoryManager = InvMgr;
		}

		if (URSEquipmentManagerComponent* EquipMgr = RSChar->FindComponentByClass<URSEquipmentManagerComponent>())
		{
			CachedEquipmentManager = EquipMgr;
		}
	}
}

ARSCharacter* URSItemManagerComponent::GetRSCharacter() const
{
	if (!CachedPawn.IsValid())
	{
		const_cast<URSItemManagerComponent*>(this)->CacheControllerAndPawn();
	}

	return CachedPawn.Get();
}

URSInventoryManagerComponent* URSItemManagerComponent::GetInventoryManager() const
{
	if (!CachedInventoryManager.IsValid())
	{
		if (ARSCharacter* RSChar = GetRSCharacter())
		{
			const_cast<URSItemManagerComponent*>(this)->CachedInventoryManager =
				RSChar->FindComponentByClass<URSInventoryManagerComponent>();
		}
	}

	return CachedInventoryManager.Get();
}

URSEquipmentManagerComponent* URSItemManagerComponent::GetEquipmentManager() const
{
	if (!CachedEquipmentManager.IsValid())
	{
		if (ARSCharacter* RSChar = GetRSCharacter())
		{
			const_cast<URSItemManagerComponent*>(this)->CachedEquipmentManager =
				RSChar->FindComponentByClass<URSEquipmentManagerComponent>();
		}
	}

	return CachedEquipmentManager.Get();
}

bool URSItemManagerComponent::EquipInventorySlotToSlotTag(int32 InventoryIndex, FGameplayTag SlotTag, FText& OutFailReason)
{
	OutFailReason = FText::GetEmpty();

	URSInventoryManagerComponent* InvMgr = GetInventoryManager();
	URSEquipmentManagerComponent* EquipMgr = GetEquipmentManager();

	if (!InvMgr || !EquipMgr)
	{
		OutFailReason = NSLOCTEXT("RS", "ItemMgr_NoManagers", "Missing inventory or equipment manager.");
		return false;
	}

	if (!InvMgr->IsValidIndex(InventoryIndex))
	{
		OutFailReason = NSLOCTEXT("RS", "ItemMgr_InvalidIndex", "Invalid inventory index.");
		return false;
	}

	URSItemInstance* Instance = InvMgr->GetItemAtIndex(InventoryIndex);
	if (!Instance)
	{
		OutFailReason = NSLOCTEXT("RS", "ItemMgr_EmptySlot", "Inventory slot is empty.");
		return false;
	}

	// 실제 장착 로직은 EquipmentManager에 위임
	if (!EquipMgr->EquipItemToSlot(SlotTag, Instance, OutFailReason))
	{
		return false;
	}

	return true;
}

bool URSItemManagerComponent::EquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason)
{
	OutFailReason = FText::GetEmpty();

	URSEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		OutFailReason = NSLOCTEXT("RS", "ItemMgr_NoEquipMgr", "No equipment manager on pawn.");
		return false;
	}

	const FGameplayTag MainWeaponSlotTag = EquipMgr->GetMainWeaponSlotTag();

	if (!MainWeaponSlotTag.IsValid())
	{
		OutFailReason = NSLOCTEXT("RS", "ItemMgr_NoMainWeaponSlotTag", "Main weapon slot tag is not set.");
		return false;
	}

	return EquipInventorySlotToSlotTag(InventoryIndex, MainWeaponSlotTag, OutFailReason);
}

bool URSItemManagerComponent::UnequipSlot(FGameplayTag SlotTag)
{
	URSEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		return false;
	}

	if (!SlotTag.IsValid())
	{
		return false;
	}

	return EquipMgr->UnequipItemFromSlot(SlotTag);
}

bool URSItemManagerComponent::UnequipMainWeapon()
{
	URSEquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr)
	{
		return false;
	}

	const FGameplayTag MainWeaponSlotTag = EquipMgr->GetMainWeaponSlotTag();
	if (!MainWeaponSlotTag.IsValid())
	{
		return false;
	}

	return EquipMgr->UnequipItemFromSlot(MainWeaponSlotTag);
}

bool URSItemManagerComponent::DropItemFromInventory(int32 InventoryIndex, int32 CountToDrop)
{
	URSInventoryManagerComponent* InvMgr = GetInventoryManager();
	if (!InvMgr)
	{
		return false;
	}

	if (!InvMgr->IsValidIndex(InventoryIndex))
	{
		return false;
	}

	URSItemInstance* Instance = InvMgr->GetItemAtIndex(InventoryIndex);
	if (!Instance)
	{
		return false;
	}

	URSItemTemplate* Template = Instance->GetTemplate();
	if (!Template)
	{
		return false;
	}

	const int32 CurrentCount = Instance->GetCount();
	if (CurrentCount <= 0)
	{
		return false;
	}

	// 드랍할 수량 결정
	int32 DropCount = CountToDrop <= 0 ? CurrentCount : FMath::Min(CurrentCount, CountToDrop);

	// 인벤토리에서 제거
	int32 Removed = 0;
	InvMgr->RemoveItemInstance(Instance, DropCount, Removed);

	if (Removed <= 0)
	{
		return false;
	}

	// 월드에 드랍 액터 스폰 (없어도 인벤토리 제거는 이미 됨)
	SpawnDropActor(Template, Removed);

	return true;
}

AActor* URSItemManagerComponent::SpawnDropActor(URSItemTemplate* ItemTemplate, int32 CountToSpawn)
{
	if (!ItemTemplate || CountToSpawn <= 0)
	{
		return nullptr;
	}

	if (!ItemPickupActorClass)
	{
		// 픽업 액터 클래스가 설정되지 않았다면, 지금은 그냥 스폰하지 않는다.
		return nullptr;
	}

	ARSCharacter* RSChar = GetRSCharacter();
	if (!RSChar)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector SpawnLocation =
		RSChar->GetActorLocation()
		+ RSChar->GetActorForwardVector() * DropOffset.X
		+ RSChar->GetActorRightVector() * DropOffset.Y
		+ FVector(0.f, 0.f, DropOffset.Z);

	const FRotator SpawnRotation = RSChar->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RSChar;
	SpawnParams.Instigator = RSChar;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Pickup = World->SpawnActor<AActor>(
		ItemPickupActorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	// 여기서 나중에:
	// - ARSItemPickupActor를 만들고
	// - Cast<ARSItemPickupActor>(Pickup)->InitializeFromTemplate(ItemTemplate, CountToSpawn);
	// 같은 초기화를 해주면 된다.

	return Pickup;
}

bool URSItemManagerComponent::RequestEquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason)
{
	// YKJ Annotation : 싱글플레이 v1은 바로 실행한다.
	return ExecuteEquipInventorySlotToMainWeapon(InventoryIndex, OutFailReason);
}

bool URSItemManagerComponent::ExecuteEquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason)
{
	return EquipInventorySlotToMainWeapon(InventoryIndex, OutFailReason);
}