#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "RSCosmeticManagerComponent.generated.h"

class ARSCharacter;
class ARSWeaponActor;
class ARSWeaponPickupActor;
class URSItemInstance;
class URSItemTemplate;
class URSItemFragment_WeaponCosmetic;



/**
 * - "보이는 것만"(외형이라고 생각하면 됨.) 책임지는 코스메틱 매니저
 * - 무기 장착/해제에 따라:
 *   - 무기 액터 스폰/파괴
 *   - 캐릭터 메시에 소켓 기준으로 Attach
 *   - (추후) 무기별 AnimLayer 전환, VFX, SFX, Trail 등 확장
 *
 *	 - 전투 로직, 스탯, Ability, 태그는 전부 RSEquipManager / ASC 쪽에서 처리
 */
UCLASS(ClassGroup = (RS), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSCosmeticManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URSCosmeticManagerComponent();

protected:
	virtual void BeginPlay() override;

	/** Owner를 ARSCharacter로 캐싱 */
	void CacheOwnerCharacter();

public:
	/**
	 * 현재 장착된 무기 외형을 주어진 ItemInstance 기반으로 교체.
	 * - ItemInstance == nullptr 이면 무기 외형 제거
	 */
// YKJ Annotation
	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void ApplyWeaponFromItem(class URSItemInstance* ItemInstance);

	/* 현재 무기 외형 제거 */
	UFUNCTION(BlueprintCallable, Category = "RS|Cosmetic")
	void ClearWeapon();

	// YKJ Annotation 
	/* 현재 스폰된 무기 액터 반환 (없으면 nullptr) */
	//UFUNCTION(BlueprintPure, Category = "RS|Cosmetic")
	//ARSWeaponActor* GetCurrentWeaponActor() const { return CurrentWeaponActor; }

protected:
	/* 내부에서 사용하는 실제 스폰/Attach 로직 */
	void SpawnAndAttachWeaponActor(const URSItemTemplate* ItemTemplate, const URSItemFragment_WeaponCosmetic* CosFrag);

	/* 이전 무기 액터 정리 */
	void DestroyCurrentWeaponActor();

protected:
	/* Owner 캐릭터 캐시 (RSCharacter 기대) */
	UPROPERTY(Transient)
	TWeakObjectPtr<ARSCharacter> CachedCharacter;

	// YKJ Annotation 
	/* 현재 스폰/Attach된 무기 액터 */
	UPROPERTY(Transient)
	TObjectPtr<ARSWeaponActor> CurrentWeaponActor;

	/* WeaponCosmetic Fragment에서 AttachSocketName이 비어 있을 때 사용할 기본 소켓 */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	FName DefaultWeaponAttachSocketName = TEXT("hand_rSocket");

	/* 무기를 해제할 때 액터를 Destroy할지, 그냥 숨길지만 결정 (지금은 Destroy 권장) */
	UPROPERTY(EditDefaultsOnly, Category = "RS|Cosmetic|Weapon")
	bool bDestroyWeaponOnUnequip = true;

	// YKJ Annotation
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RS|Cosmetic")
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
};
