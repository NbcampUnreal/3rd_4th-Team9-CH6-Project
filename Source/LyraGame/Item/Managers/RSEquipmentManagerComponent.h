#pragma once

#include "RSDefine.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "RSEquipmentManagerComponent.generated.h"

//class URSEquipManagerComponent;
class ALyraCharacter;
class ALyraPlayerController;
class URSItemInstance;
class URSItemTemplate;
//class URSEquipmentManagerComponent;
//class URSInventoryManagerComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnEquipmentEntryChanged, EEquipmentSlotType, URSItemInstance*, int32/*ItemCount*/);

USTRUCT(BlueprintType)
struct FRSEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	void Init(URSItemInstance* InItemInstance, int32 InItemCount);
	URSItemInstance* Reset();

public:
	URSItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }

private:
	friend struct FRSEquipmentList;
	friend class URSEquipmentManagerComponent;
	friend class URSItemManagerComponent;

	UPROPERTY()
	TObjectPtr<URSItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

private:
	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY(NotReplicated)
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;
};

USTRUCT(BlueprintType)
struct FRSEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FRSEquipmentList() : EquipmentManager(nullptr) {}
	FRSEquipmentList(URSEquipmentManagerComponent* InOwnerComponent) : EquipmentManager(InOwnerComponent) {}

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount);

public:
	const TArray<FRSEquipmentEntry>& GetAllEntries() const { return Entries; }

private:
	friend class URSEquipmentManagerComponent;
	friend class URSInventoryManagerComponent;
	friend class URSItemManagerComponent;

	UPROPERTY()
	TArray<FRSEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;
};

template<>
struct TStructOpsTypeTraits<FRSEquipmentList> : public TStructOpsTypeTraitsBase2<FRSEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS(BlueprintType)
class URSEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	URSEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

public:
	/*
	int32 CanMoveOrMergeEquipment(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;

	int32 CanMoveOrMergeEquipment_Quick(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment_Quick(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment_Quick(int32 FromItemTemplateID, EItemRarity FromItemRarity, int32 FromItemCount, EEquipmentSlotType& OutToEquipmentSlotType) const;

	bool CanSwapEquipment(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	bool CanSwapEquipment(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType, FIntPoint& OutToItemSlotPos);

	bool CanSwapEquipment_Quick(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanSwapEquipment_Quick(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType, FIntPoint& OutToItemSlotPos);
	*/

	int32 CanAddEquipment(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, EEquipmentSlotType ToEquipmentSlotType) const;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddUnarmedEquipments(TSubclassOf<URSItemTemplate> LeftHandClass, TSubclassOf<URSItemTemplate> RightHandClass);

public:
	void AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount);
	URSItemInstance* RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, int32 ItemCount);
	void SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<URSItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount);

public:
	static bool IsWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsArmorSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsUtilitySlot(EEquipmentSlotType EquipmentSlotType);

	static bool IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState);
	static bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType);
	static bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType);

	static bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);

	const URSItemInstance* FindPairItemInstance(const URSItemInstance* InBaseItemInstance, EEquipmentSlotType& OutEquipmentSlotType) const;

	bool IsAllEmpty(EEquipState EquipState) const;

	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	//URSEquipManagerComponent* GetEquipManager() const;

	URSItemInstance* GetItemInstance(EEquipmentSlotType EquipmentSlotType) const;
	int32 GetItemCount(EEquipmentSlotType EquipmentSlotType) const;

	const TArray<FRSEquipmentEntry>& GetAllEntries() const;
	void GetAllWeaponItemInstances(TArray<URSItemInstance*>& OutItemInstances) const;

public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;

private:
	friend class URSItemManagerComponent;
	friend class ULyraCheatManager;

	UPROPERTY(Replicated)
	FRSEquipmentList EquipmentList;
};
