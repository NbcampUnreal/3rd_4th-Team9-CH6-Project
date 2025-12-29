#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RSItemManagerComponent.generated.h"

class ARSCharacter;
class URSInventoryManagerComponent;
class URSEquipmentManagerComponent;
class URSItemInstance;
class URSItemTemplate;

/**
 * RSItemManagerComponent
 *
 * - PlayerController에 붙어서 인벤토리/장비/드랍을 “고수준 오케스트레이션”하는 매니저
 * - 실제 데이터는 Pawn 쪽 매니저들이 관리:
 *   - 인벤토리: URSInventoryManagerComponent (Pawn)
 *   - 장비:     URSEquipmentManagerComponent (Pawn)
 *   - GAS/스탯: URSEquipManagerComponent (Pawn)
 *   - 코스메틱: URSCosmeticManagerComponent (Pawn)
 *
 * - 이 컴포넌트는:
 *   - 인벤토리에서 슬롯 선택 → 장착/해제 호출
 *   - 아이템 드랍 요청 → 픽업 액터 스폰
 *   - 나중에 UI/RPC/입력 시스템이 여기로 들어오게 될 예정
 */
UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSItemManagerComponent();

protected:
	virtual void BeginPlay() override;

	/** Owner(Controller)와 현재 Possess 중인 RSCharacter를 캐싱 */
	void CacheControllerAndPawn();

public:
	/** 현재 Possess 중인 RSCharacter 반환 (없으면 nullptr) */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	ARSCharacter* GetRSCharacter() const;

	/** 현재 Pawn의 InventoryManager 가져오기 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	URSInventoryManagerComponent* GetInventoryManager() const;

	/** 현재 Pawn의 EquipmentManager 가져오기 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	URSEquipmentManagerComponent* GetEquipmentManager() const;

	/**
	 * 인벤토리 슬롯의 아이템을 지정 슬롯(보통 메인 무기 슬롯)에 장착 시도.
	 *
	 * - InventoryIndex: 인벤토리 인덱스
	 * - SlotTag: 장비 슬롯 태그 (예: Slot.Weapon.Main)
	 * - OutFailReason: 실패 이유 텍스트
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Equip")
	bool EquipInventorySlotToSlotTag(int32 InventoryIndex, FGameplayTag SlotTag, FText& OutFailReason);

	/**
	 * 메인 무기 슬롯에 인벤토리 아이템 장착 (헬퍼).
	 * - MainWeaponSlotTag는 EquipmentManager에 설정되어 있다고 가정.
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Equip")
	bool EquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason);

	/**
	 * 지정 장비 슬롯 해제
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Equip")
	bool UnequipSlot(FGameplayTag SlotTag);

	/**
	 * 메인 무기 슬롯 해제 (헬퍼)
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Equip")
	bool UnequipMainWeapon();

	/**
	 * 인벤토리 슬롯의 아이템 일부/전체를 드랍.
	 *
	 * - CountToDrop <= 0 이면 해당 스택 전부 드랍
	 * - 실제 드랍 액터는 ItemPickupActorClass로 스폰
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Drop")
	bool DropItemFromInventory(int32 InventoryIndex, int32 CountToDrop);

	// YKJ Annotation : UI/입력은 Request만 부른다.
	UFUNCTION(BlueprintCallable, Category = "RS|Item|Equip")
	bool RequestEquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason);

protected:
	/** 실제로 드랍 액터를 스폰하는 내부 헬퍼 */
	AActor* SpawnDropActor(URSItemTemplate* ItemTemplate, int32 CountToSpawn);

	// YKJ Annotation : 실제 실행(싱글에서는 즉시 호출). 멀티 전환 시 서버 실행 함수로 바뀔 자리.
	bool ExecuteEquipInventorySlotToMainWeapon(int32 InventoryIndex, FText& OutFailReason);

protected:
	/** 캐시된 컨트롤러 (Owner) */
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> CachedPC;

	/** 캐시된 현재 RSCharacter (Possessed Pawn) */
	UPROPERTY(Transient)
	TWeakObjectPtr<ARSCharacter> CachedPawn;

	/** 인벤토리/장비 매니저 캐시 (Pawn에서 찾음) */
	UPROPERTY(Transient)
	TWeakObjectPtr<URSInventoryManagerComponent> CachedInventoryManager;

	UPROPERTY(Transient)
	TWeakObjectPtr<URSEquipmentManagerComponent> CachedEquipmentManager;

	/**
	 * 드랍 시 사용할 픽업 액터 클래스
	 * - 일단 AActor 베이스로 두고,
	 *   나중에 ARSItemPickupActor를 만들면 여기다가 넣고
	 *   SpawnDropActor 안에서 Cast해서 Initialize 해도 된다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Item|Drop")
	TSubclassOf<AActor> ItemPickupActorClass;

	/** 드랍 시 스폰 위치 오프셋 (Pawn 기준) */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Item|Drop")
	FVector DropOffset = FVector(100.f, 0.f, 50.f);
};
